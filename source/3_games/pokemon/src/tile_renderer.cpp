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
    upload_vertex_buffer();

    auto render_ctx = rex::gfx::new_render_ctx();

    m_render_pass->bind_to(render_ctx.get());

    // Bind all the resources to the gfx pipeline
    render_ctx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);

    render_ctx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
    render_ctx->set_vertex_buffer(m_tiles_instances_immutable_vb_gpu.get(), 1);
    render_ctx->set_vertex_buffer(m_tiles_instances_writeable_vb_gpu.get(), 2);
    render_ctx->set_index_buffer(m_tiles_ib_gpu.get());

    //#TODO: Get rid of these 2 calls
    render_ctx->set_viewport(m_viewport);
    render_ctx->set_scissor_rect(m_scissor_rect);

    // Send the draw command
    const s32 index_count_per_instance = 6;
    const s32 instance_count = num_tiles();
    render_ctx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
  }

  void TileRenderer::update_tileset_texture(const rex::gfx::Texture2D* texture)
  {
    m_tileset_texture = texture;

    // Initialize/update all resources that depend on the tileset texture in some way
    init_cb();
    init_per_vertex_vb();

    m_render_pass->set("tile_texture", m_tileset_texture);
  }
  void TileRenderer::update_dimension(s32 width, s32 height)
  {
    m_width = width;
    m_height = height;

    // Reinitialize all resources that change based on the dimension that needs to be rendered
    m_tiles_instances_writeable_vb_gpu = rex::gfx::rhi::create_vertex_buffer(num_tiles(), sizeof(TileMutableInstanceData));
    m_tile_cache = rsl::make_unique<s32[]>(num_tiles());

    init_per_instance_immutable_vb();
  }

  void TileRenderer::update_tile_data(const MapMatrix& mapMatrix, TileCoord playerPos)
	{
    s32 tiles_per_row = m_tileset_texture->width() / constants::g_tile_width_px;
    f32 inv_texture_width = 1.0f / m_tileset_texture->width();
    f32 inv_texture_height = 1.0f / m_tileset_texture->height();

    rsl::sort(m_tile_cache.begin(), m_tile_cache.end());
    auto x = rsl::unique(m_tile_cache.begin(), m_tile_cache.end());
    auto d = rsl::distance(m_tile_cache.begin(), x);

    REX_INFO(LogTileRenderer, "Unique tile indices: {}", d);

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
        rsl::pointi8 coord_rel_to_block = coord - block_top_left;
        u8 tile_idx = block.index_at(coord_rel_to_block);

        // Store the tile index in the cache
        m_tile_cache[current_tile_in_cache_idx] = tile_idx;
        rsl::pointi8 tile_coord = coords::index_to_coord(tile_idx, tiles_per_row);

        // Top left coordinate in UV space of the tile to be rendered
        glm::vec2 tile_uv_start{};
        tile_uv_start.x = constants::g_tile_width_px * tile_coord.x * inv_texture_width;
        tile_uv_start.y = constants::g_tile_height_px * tile_coord.y * inv_texture_height;

        //m_tile_coords[current_tile_in_cache_idx] = tile_uv_start;

        current_tile_in_cache_idx++;
      }
    }
	}

  s32 TileRenderer::num_tiles() const
  {
    return m_width * m_height;
  }

  void TileRenderer::init()
  {
    // The writeable per isntance vertex buffer doesn't have any data in the beginning
    // The data for this buffer gets updated every frame.
    m_tiles_instances_writeable_vb_gpu = rex::gfx::rhi::create_vertex_buffer(num_tiles(), sizeof(TileMutableInstanceData));

    // The tile cache is like a map matrix but it holds indices on a tile level instead of block level
    m_tile_cache = rsl::make_unique<s32[]>(num_tiles());

    //#TODO: create a viewport and scissor rect for the backbuffer
    m_viewport.top_left_x = 0.0f;
    m_viewport.top_left_y = 0.0f;
    m_viewport.width = 720.0f;
    m_viewport.height = 720.0f;

    m_scissor_rect.right = m_viewport.width;
    m_scissor_rect.bottom = m_viewport.height;

    // Initialize all the GPU resources
    init_per_vertex_vb();
    init_per_instance_immutable_vb();
    init_ib();
    init_cb();
    init_sampler();
    init_renderpass();
  }
  void TileRenderer::init_per_vertex_vb()
  {
    f32 uv_width = static_cast<f32>(constants::g_tile_width_px) / m_tileset_texture->width();
    f32 uv_height = static_cast<f32>(constants::g_tile_height_px) / m_tileset_texture->height();

    constexpr s32 num_vertices_per_tile = 4;

    rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};
    // A single tile starts from the top left and spans the entire screen
    // NDC coordinates for a screen go from -1 to 1, meaning that width and height should be of a unit of 2
    tile_vertices[0] = TileVertex{rsl::point<f32>(0, 0), rsl::point<f32>(0.0f, 0.0f)};            // top left
    tile_vertices[1] = TileVertex{rsl::point<f32>(2, 0), rsl::point<f32>(uv_width, 0.0f)};        // top right
    tile_vertices[2] = TileVertex{rsl::point<f32>(0, -2), rsl::point<f32>(0.0f, uv_height)};      // bottom left
    tile_vertices[3] = TileVertex{rsl::point<f32>(2, -2), rsl::point<f32>(uv_width, uv_height)};  // bottom right

    m_tiles_vb_gpu = rex::gfx::rhi::create_vertex_buffer(num_vertices_per_tile, sizeof(TileVertex));

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(TileVertex));
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);

  }
  void TileRenderer::init_per_instance_immutable_vb()
  {
    rsl::unique_array<TileReadonlyInstanceData> tile_world_matrices = rsl::make_unique<TileReadonlyInstanceData[]>(num_tiles());

    constexpr f32 inv_tile_screen_width = 1.0f / constants::g_screen_width_in_tiles;
    constexpr f32 inv_tile_screen_height = 1.0f / constants::g_screen_height_in_tiles;

    constexpr s32 tile_width = 2;
    constexpr s32 tile_height = 2;

    s32 tile_count = num_tiles();
    for (s32 i = 0; i < tile_count; ++i)
    {
      // Calculate the position of the current tile
      rsl::pointi8 coord = coords::index_to_coord(i, m_width);

      // Create the world matrix from the world position
      glm::mat4 world(1.0f);
      glm::vec3 pos{ -1, 1, 0 }; // Top left position

      // Offset the tile from the top left position
      pos.x += coord.x * tile_width * inv_tile_screen_width;
      pos.y -= coord.y * tile_height * inv_tile_screen_height;

      // create the world matrix, putting the tile at the right position and at the right scale
      world = glm::translate(world, pos);
			world = glm::scale(world, glm::vec3(inv_tile_screen_width, inv_tile_screen_height, 1.0f));
      tile_world_matrices[i].world = world;
      tile_world_matrices[i].world = glm::transpose(tile_world_matrices[i].world); // of course we're using directx, so we always have to transpose our matrices
    }

    m_tiles_instances_immutable_vb_gpu = rex::gfx::rhi::create_vertex_buffer(num_tiles(), sizeof(TileReadonlyInstanceData));

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_instances_immutable_vb_gpu.get(), tile_world_matrices.get(), tile_world_matrices.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);
    
    auto render_ctx = rex::gfx::new_render_ctx();
    render_ctx->transition_buffer(m_tiles_instances_immutable_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
  }
  void TileRenderer::init_ib()
  {
    constexpr s32 num_indices_per_tile = 6;
    rsl::unique_array<u16> tiles_ib_cpu = rsl::make_unique<u16[]>(num_indices_per_tile);

    // first triangle
    tiles_ib_cpu[0] = 0;
    tiles_ib_cpu[1] = 1;
    tiles_ib_cpu[2] = 2;

    // second triangle
    tiles_ib_cpu[3] = 1;
    tiles_ib_cpu[4] = 3;
    tiles_ib_cpu[5] = 2;

    m_tiles_ib_gpu = rex::gfx::rhi::create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_ib_gpu.get(), tiles_ib_cpu.get(), tiles_ib_cpu.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);

    auto render_ctx = rex::gfx::new_render_ctx();
    render_ctx->transition_buffer(m_tiles_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
  }
  void TileRenderer::init_cb()
  {
    TileTextureMetaData texture_data{};
    texture_data.tiles_per_row = m_tileset_texture->width() / constants::g_tile_width_px;
    texture_data.inv_width = 1.0f / m_tileset_texture->width();
    texture_data.inv_height = 1.0f / m_tileset_texture->height();

    m_tile_renderer_cb = rex::gfx::rhi::create_constant_buffer(sizeof(TileTextureMetaData));

    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tile_renderer_cb.get(), &texture_data, sizeof(texture_data));
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);
  }
  void TileRenderer::init_sampler()
  {
    // Sampler is currently hardcoded
    //#TODO: Create default samplers
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

    m_default_sampler = rex::gfx::rhi::create_sampler2d(desc);
  }

  void TileRenderer::init_renderpass()
  {
    rex::gfx::RenderPassDesc render_pass_desc{};

    render_pass_desc.name = "Tile Renderer";

    //#TODO: Create a default render state here
    rex::gfx::RasterStateDesc& raster_state = render_pass_desc.pso_desc.output_merger.raster_state;
    raster_state.fill_mode = rex::gfx::FillMode::Solid;
    raster_state.cull_mode = rex::gfx::CullMode::Back;
    raster_state.front_ccw = false;
    raster_state.depth_clip_enable = true;

    // We're rendering directly to the back buffer
    render_pass_desc.framebuffer_desc.emplace_back(rex::gfx::swapchain_frame_buffer_handle());

    // Assign the shaders used for the tile renderer
    render_pass_desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::load(rex::path::join(rex::vfs::project_root(), "retail", "shaders", "render_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
    render_pass_desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::load(rex::path::join(rex::vfs::project_root(), "retail", "shaders", "render_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

    // The tile renderer uses 3 vertex buffers
    // 1 vertex buffer holding per vertex data, 2 flaots for position, 2 for UV
    // 1 vertex buffer holding the world matrix for each tile, this is immutable after initialization
    // 1 vertex buffer holding the uv offset for each tile, this gets updated every frame
    render_pass_desc.pso_desc.input_layout =
    {
      // Per vertex data
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::VertexBufferFormat::Float2 },
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::VertexBufferFormat::Float2 },

      // Immutable per instance data
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 0, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 1, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 2, 1, -1, 1 }, // WVPs
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceMatrix, rex::gfx::VertexBufferFormat::Float4, rex::gfx::InputLayoutClassification::PerInstance, 3, 1, -1, 1 }, // WVPs

      // Writeable per instance data
      rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::InstanceIndex, rex::gfx::VertexBufferFormat::Uint, rex::gfx::InputLayoutClassification::PerInstance, 0, 2, -1, 1 } // Tile index
    };

    m_render_pass = rsl::make_unique<rex::gfx::RenderPass>(render_pass_desc);
    m_render_pass->set("tile_texture", m_tileset_texture);
    m_render_pass->set("default_sampler", m_default_sampler.get());
    m_render_pass->set("TileTextureMetaData", m_tile_renderer_cb.get());
  }

  void TileRenderer::upload_vertex_buffer()
  {
    auto copy_ctx = rex::gfx::new_copy_ctx();
    copy_ctx->update_buffer(m_tiles_instances_writeable_vb_gpu.get(), m_tile_cache.get(), m_tile_cache.byte_size());
    copy_ctx->execute_on_gpu(rex::gfx::WaitForFinish::yes);
  }
}