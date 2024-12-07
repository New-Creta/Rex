#include "pokemon/tile_renderer.h"

#include "pokemon/render_constants.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/gfx/system/shader_library.h"

#include "glm/gtc/matrix_transform.hpp"

DEFINE_LOG_CATEGORY(LogTileRenderer);

namespace pokemon
{
  TileRenderer::TileRenderer(const TileRendererDesc& desc)
    : m_width(desc.width_in_tiles)
    , m_height(desc.height_in_tiles)
    , m_tileset_texture(desc.tileset_texture)
    , m_blockset(desc.blockset)
  {
    init();
  }

  void TileRenderer::new_frame()
  {
    // Nothing to implement
  }
  void TileRenderer::render()
  {
    // Update the vertex buffer with the latest data
    update_cpu_vertex_buffer();
    upload_vertex_buffer();

    auto render_ctx = rex::gfx::new_render_ctx();

    m_render_pass->bind_to(render_ctx.get());

    // Bind all the resources to the gfx pipeline
    render_ctx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);

    render_ctx->set_viewport(m_viewport);
    render_ctx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
    render_ctx->set_vertex_buffer(m_tiles_instances_vb_gpu.get(), 1);
    render_ctx->set_index_buffer(m_tiles_ib_gpu.get());

    render_ctx->set_scissor_rect(m_scissor_rect);

    // Send the draw command
    const s32 index_count_per_instance = 6;
    const s32 instance_count = 360;
    render_ctx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
  }

  void TileRenderer::update_tileset_texture(const rex::gfx::Texture2D* texture)
  {
    m_tileset_texture = texture;
  }
  void TileRenderer::update_dimension(s32 width, s32 height)
  {
    m_width = width;
    m_height = height;

    init();
  }

  void TileRenderer::update_tile_data(const MapMatrix& mapMatrix, TileCoord playerPos)
	{
    // Calculate the first block from which we should start drawing
    TileCoord top_left = coords::player_pos_to_screen_top_left(playerPos);

    // Start the loop from this block, going left to right, top to down
    // Restricting to only the tiles that'll be rendered
    s32 current_tile_in_cache_idx = 0;
    for (s32 y = 0; y < m_height; ++y)
    {
      for (s32 x = 0; x < m_width; ++x)
      {
        // Get the tile coord of the tile we're currently processing
        TileCoord coord = top_left;
        coord.x += x;
        coord.y += y;

        // Get the block the tile belongs to
        s32 block_idx = mapMatrix.index_at(coord);
        const Block& block = m_blockset->block(block_idx);

        // Get the tile coordinate of the the first tile in the block (which is top left)
        TileCoord block_top_left = coords::block_top_left_coord(coord);

        // Get the relative vector from this first tile to the current tile
        // Based on that, calculate which tile in the block we're currently processing
        s32 tile_idx_in_block = coords::coord_to_index(coord - block_top_left, constants::g_num_tiles_per_block_row);
        u8 tile_idx = block.index_at(tile_idx_in_block);

        // Store the tile index in the cache
        m_tile_cache[current_tile_in_cache_idx++] = tile_idx;
      }
    }
	}

  s32 TileRenderer::expected_num_vertices() const
  {
    constexpr s32 num_vertices_per_tile = 4;
    return m_width * m_height * num_vertices_per_tile;
  }
  s32 TileRenderer::expected_num_tiles() const
  {
    return m_width * m_height;
  }
  s32 TileRenderer::expected_num_wvps() const
  {
    return expected_num_tiles();
  }
  s32 TileRenderer::expected_num_indices() const
  {
    constexpr s32 num_indices_per_tile = 6;
    return expected_num_tiles() * num_indices_per_tile;
  }

  void TileRenderer::init()
  {
    // This is the only data that needs to get updated per frame
    m_tiles_vb_cpu = rsl::make_unique<TileVertex[]>(expected_num_vertices());

    // GPU vertex and index buffers
    m_tiles_vb_gpu = rex::gfx::rhi::create_vertex_buffer(expected_num_vertices(), sizeof(TileVertex));
    m_tiles_instances_vb_gpu = rex::gfx::rhi::create_vertex_buffer(expected_num_tiles(), sizeof(TileWVP));
    m_tiles_ib_gpu = rex::gfx::rhi::create_index_buffer(expected_num_indices(), rex::gfx::IndexBufferFormat::Uint16);

    // The tile cache is like a map matrix but it holds indices on a tile level instead of block level
    m_tile_cache = rsl::make_unique<u8[]>(m_width * m_height);

    m_viewport.top_left_x = 0.0f;
    m_viewport.top_left_y = 0.0f;
    m_viewport.width = 1280.0f;
    m_viewport.height = 720.0f;

    m_scissor_rect.right = m_viewport.width;
    m_scissor_rect.bottom = m_viewport.height;

    // The vertex buffer for instances and index buffer never change. 
    // Only the vertices itself do, well their UV coordinate
    // So we can initialize the immutable buffers early on to save on runtime
    init_instance_vb();
    init_ib();
    init_renderpass();
  }
  void TileRenderer::init_instance_vb()
  {
    rsl::unique_array<TileWVP> tile_wvps = rsl::make_unique<TileWVP[]>(expected_num_tiles());

    constexpr f32 inv_tile_screen_width = 1.0f / constants::g_screen_width_in_tiles;
    constexpr f32 inv_tile_screen_height = 1.0f / constants::g_screen_height_in_tiles;

    for (s32 i = 0, v = 0; i < expected_num_tiles(); ++i)
    {
      // Calculate the position of the current tile
      rsl::pointi8 coord = coords::index_to_coord(i, m_width);

      // Create the world matrix from the world position
      glm::mat4 world(1.0f);
      glm::vec3 pos{ -1, 1, 0 };
      pos.x += coord.x * 2 * inv_tile_screen_width;
      pos.y -= coord.y * 2 * inv_tile_screen_height;
      world = glm::translate(world, pos);
			world = glm::scale(world, glm::vec3(inv_tile_screen_width, inv_tile_screen_height, 1.0f));
      tile_wvps[i].wvp = world;
      tile_wvps[i].wvp = glm::transpose(tile_wvps[i].wvp);
    }

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_instances_vb_gpu.get(), tile_wvps.get(), tile_wvps.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);
    
    auto render_ctx = rex::gfx::new_render_ctx();
    render_ctx->transition_buffer(m_tiles_instances_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
  }
  void TileRenderer::init_ib()
  {
    rsl::unique_array<u16> tiles_ib_cpu = rsl::make_unique<u16[]>(expected_num_indices());

    constexpr s32 num_vertices_per_tile = 4;
    constexpr s32 num_indices_per_tile = 6;
    for (s32 i = 0, v = 0; i < expected_num_tiles(); i += num_indices_per_tile, v += num_vertices_per_tile)
    {
      // First triangle
      tiles_ib_cpu[i + 0] = v;
      tiles_ib_cpu[i + 1] = v + 1;
      tiles_ib_cpu[i + 2] = v + 2;

      // Second triangle
      tiles_ib_cpu[i + 3] = v + 1;
      tiles_ib_cpu[i + 4] = v + 3;
      tiles_ib_cpu[i + 5] = v + 2;
    }

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_ib_gpu.get(), tiles_ib_cpu.get(), tiles_ib_cpu.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);

    auto render_ctx = rex::gfx::new_render_ctx();
    render_ctx->transition_buffer(m_tiles_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
  }

  void TileRenderer::init_renderpass()
  {
    rex::gfx::RenderPassDesc render_pass_desc{};

    render_pass_desc.name = "Tile Renderer";

    rex::gfx::RasterStateDesc& raster_state = render_pass_desc.pso_desc.output_merger.raster_state;
    raster_state.fill_mode = rex::gfx::FillMode::Solid;
    raster_state.cull_mode = rex::gfx::CullMode::None;
    raster_state.front_ccw = false;
    raster_state.depth_clip_enable = true;

    render_pass_desc.framebuffer_desc.emplace_back(rex::gfx::swapchain_frame_buffer_handle());

    render_pass_desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::load(rex::path::join(rex::vfs::project_root(), "retail", "shaders", "render_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
    render_pass_desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::load(rex::path::join(rex::vfs::project_root(), "retail", "shaders", "render_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

    render_pass_desc.pso_desc.input_layout =
    {
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::VertexBufferFormat::Float2 },
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::VertexBufferFormat::Float2 },
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 0, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 1, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 2, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 3, 1, -1, 1 } // WVPs
    };

    // Sampler is currently hardcoded
    rex::gfx::SamplerDesc desc{};
    desc.filtering = rex::gfx::SamplerFiltering::MinMagMipPoint;
    desc.address_mode_u = rex::gfx::TextureAddressMode::Wrap;
    desc.address_mode_v = rex::gfx::TextureAddressMode::Wrap;
    desc.address_mode_w = rex::gfx::TextureAddressMode::Wrap;
    desc.mip_lod_bias = 0.0f;
    desc.max_anisotropy = 0;
    desc.comparison_func = rex::gfx::ComparisonFunc::Always;
    desc.border_color = rex::gfx::BorderColor::TransparentBlack;
    desc.min_lod = 0.0f;
    desc.max_lod = 0.0f;
    desc.shader_register = 0;
    desc.register_space = 0;
    desc.shader_visibility = rex::gfx::ShaderVisibility::Pixel;

    m_fonts_sampler = rex::gfx::rhi::create_sampler2d(desc);

    m_render_pass = rsl::make_unique<rex::gfx::RenderPass>(render_pass_desc);
    m_render_pass->set("tile_texture", m_tileset_texture);
    m_render_pass->set("default_sampler", m_fonts_sampler.get());

  }

  void TileRenderer::update_cpu_vertex_buffer()
  {
    f32 uv_width = static_cast<f32>(constants::g_tile_width_px) / m_tileset_texture->width();
    f32 uv_height = static_cast<f32>(constants::g_tile_height_px) / m_tileset_texture->height();

    s32 tiles_per_row = m_tileset_texture->width() / constants::g_tile_width_px;

    // Technically we can allow the position of the vertices to be set at initialization time
    // As they don't change during runtime

    for (s32 idx = 0; idx < m_tile_cache.count(); ++idx)
    {
      u8 tile_index = m_tile_cache[idx];
      rsl::pointi8 coord = coords::index_to_coord(tile_index, tiles_per_row);

      constexpr s32 num_vertices_per_tile = 4;
      s32 vb_idx = num_vertices_per_tile * idx;

      // Top Left Vertex
      TileVertex& top_left_vtx = m_tiles_vb_cpu[vb_idx + 0];
      top_left_vtx.pos = { -1, 1 };
      top_left_vtx.uv.x = (coord.x * constants::g_tile_width_px) / static_cast<f32>(m_tileset_texture->width());
      top_left_vtx.uv.y = (coord.y * constants::g_tile_height_px) / static_cast<f32>(m_tileset_texture->height());

      // Top Right Vertex
      TileVertex& top_right_vtx = m_tiles_vb_cpu[vb_idx + 1];
      top_right_vtx.pos = { 1, 1 };
      top_right_vtx.uv = top_left_vtx.uv;
      top_right_vtx.uv.x += uv_width;

      // Bottom Left Vertex
      TileVertex& bottom_left_vtx = m_tiles_vb_cpu[vb_idx + 2];
      bottom_left_vtx.pos = { -1, -1 };
      bottom_left_vtx.uv = top_left_vtx.uv;
      bottom_left_vtx.uv.y += uv_height;

      // Bottom Right Vertex
      TileVertex& bottom_right_vtx = m_tiles_vb_cpu[vb_idx + 3];
      bottom_right_vtx.pos = { 1, -1 };
      bottom_right_vtx.uv = top_left_vtx.uv;
      bottom_right_vtx.uv.x += uv_width;
      bottom_right_vtx.uv.y += uv_height;
    }
  }

  void TileRenderer::upload_vertex_buffer()
  {
    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_vb_gpu.get(), m_tiles_vb_cpu.get(), m_tiles_vb_cpu.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);
  }
}