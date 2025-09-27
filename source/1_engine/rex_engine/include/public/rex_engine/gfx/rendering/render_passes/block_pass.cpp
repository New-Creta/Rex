#include "rex_engine/gfx/rendering/render_passes/block_pass.h"

#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/filesystem/path.h"

namespace rex
{
	namespace gfx
	{
		struct TileVertex
		{
			rsl::point<f32> pos;
			rsl::point<f32> uv;
		};

		BlockRenderPass::BlockRenderPass(rex::gfx::RenderTargetBase* rt, const rex::TilesetAsset* tileset)
			: m_render_target(rt)
			, m_tileset(tileset)
			, m_tile_zoom(1.0f)
		{
			init();
		}

		void BlockRenderPass::set_render_target(RenderTargetBase* render_target)
		{
			m_render_target = render_target;
			init();
		}

		void BlockRenderPass::set_tileset(const rex::TilesetAsset* tileset)
		{
			m_tileset = tileset;
			init();
		}
		void BlockRenderPass::set_tile_zoom(f32 zoom)
		{
			m_tile_zoom = zoom;
			init();
		}

		void BlockRenderPass::render(rex::gfx::RenderContext* renderCtx)
		{
			if (!m_tilemap)
			{
				return;
			}

			const s32 tile_byte_size = sizeof(m_tilemap->tiles()[0]);
			renderCtx->update_buffer(m_tiles_indices_buffer.get(), m_tilemap->tiles(), m_tilemap->num_tiles() * tile_byte_size);
			renderCtx->transition_buffer(m_tiles_indices_buffer.get(), rex::gfx::ResourceState::NonPixelShaderResource);

			m_render_pass->bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());
			renderCtx->set_render_target(m_render_target);
			renderCtx->clear_render_target(m_render_target);

			s32 render_target_width = m_render_target->width();
			s32 render_target_height = m_render_target->height();

			f32 viewport_width = static_cast<f32>(render_target_width);
			f32 viewport_height = static_cast<f32>(render_target_height);
			rex::gfx::Viewport viewport = { glm::vec2(0.0f, 0.0f), viewport_width, viewport_height, 0.0f, 1.0f };
			renderCtx->set_viewport(viewport);

			rex::gfx::ScissorRect rect = { 0, 0, viewport_width, viewport_height };
			renderCtx->set_scissor_rect(rect);

			// Send the draw command
			const s32 index_count_per_instance = 6;
			const s32 instance_count = m_tilemap->num_tiles();
			renderCtx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
		}

		void BlockRenderPass::init()
		{
			if (m_tilemap == nullptr || m_tileset == nullptr)
			{
				return;
			}

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_tile_info(render_ctx.get());
			init_tile_indices_uab(render_ctx.get());
			init_render_pass();
		}

		void BlockRenderPass::init_tile_info(rex::gfx::RenderContext* renderCtx)
		{
			// Perform the calculations required to initialize the VB and other render data
			s32 render_target_width = m_render_target->width();
			s32 render_target_height = m_render_target->height();
			rsl::pointi8 tile_size = m_tileset->tile_size();

			f32 zoomed_tile_size_x = tile_size.x;
			f32 zoomed_tile_size_y = tile_size.y;
			zoomed_tile_size_x *= m_tile_zoom;
			zoomed_tile_size_y *= m_tile_zoom;

			s32 render_target_width_in_tiles = render_target_width / zoomed_tile_size_x;
			s32 render_target_height_in_tiles = render_target_height / zoomed_tile_size_y;

			// inverse tile width comes from diving from 2
			// this is because ndc coordinates have a width of 2 (going from -1 to 1)
			f32 inv_tile_width = 2.0f / render_target_width_in_tiles;
			f32 inv_tile_height = 2.0f / render_target_height_in_tiles;

			s32 tileset_width = m_tileset->tileset_texture()->texture_resource()->width();
			s32 tileset_height = m_tileset->tileset_texture()->texture_resource()->height();

			rsl::vec2 inv_tile_size{ inv_tile_width, inv_tile_height };
			rsl::vec2 uv_size{};
			uv_size.x = tile_size.x / (f32)tileset_width;
			uv_size.y = tile_size.y / (f32)tileset_height;

			init_vb(renderCtx, inv_tile_size, uv_size);
			init_ib(renderCtx);
			init_render_info(renderCtx, inv_tile_size, uv_size);
		}

		void BlockRenderPass::init_vb(rex::gfx::RenderContext* renderCtx, rsl::vec2 invTileSize, rsl::vec2 uvSize)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};

			tile_vertices[0] = TileVertex{ rsl::point<f32>(0,										0),               rsl::point<f32>(0.0f,     0.0f) };
			tile_vertices[1] = TileVertex{ rsl::point<f32>(invTileSize.x,			  0),               rsl::point<f32>(uvSize.x, 0.0f) };
			tile_vertices[2] = TileVertex{ rsl::point<f32>(0,									 -invTileSize.y),		rsl::point<f32>(0.0f,     uvSize.y) };
			tile_vertices[3] = TileVertex{ rsl::point<f32>(invTileSize.x,			 -invTileSize.y),		rsl::point<f32>(uvSize.x, uvSize.y) };

			if (!m_tiles_vb_gpu)
			{
				m_tiles_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(TileVertex));
			}

			renderCtx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(TileVertex));
			renderCtx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void BlockRenderPass::init_ib(rex::gfx::RenderContext* renderCtx)
		{
			if (m_tiles_ib_gpu)
			{
				return;
			}

			const s32 num_indices_per_tile = 6;
			rsl::array<u16, 6> tile_ib{};

			tile_ib[0] = 0;
			tile_ib[1] = 1;
			tile_ib[2] = 2;

			tile_ib[3] = 1;
			tile_ib[4] = 3;
			tile_ib[5] = 2;

			m_tiles_ib_gpu = rex::gfx::gal::instance()->create_index_buffer(num_indices_per_tile, rex::gfx::IndexBufferFormat::Uint16);

			// create the constant buffer
			// -----------------------------------------
			renderCtx->update_buffer(m_tiles_ib_gpu.get(), tile_ib.data(), tile_ib.size() * sizeof(tile_ib[0]));
			renderCtx->transition_buffer(m_tiles_ib_gpu.get(), rex::gfx::ResourceState::IndexBuffer);
		}
		void BlockRenderPass::init_render_info(rex::gfx::RenderContext* renderCtx, rsl::vec2 invTileSize, rsl::vec2 uvSize)
		{
			// Init the constant buffer
			struct TilemapRenderingMetaData
			{
				// Tile texture data
				u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
				f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
				f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

				// Render target data
				u32 screen_width_in_tiles;   // the number of tiles we render on a single row
				f32 inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
				f32 inv_tile_screen_height;  // the inverse of the height of a single tile on the screen
			};

			TilemapRenderingMetaData render_metadata{};
			render_metadata.texture_tiles_per_row = m_tileset->tile_size_per_row();
			render_metadata.inv_texture_width = uvSize.x;
			render_metadata.inv_texture_height = uvSize.y;

			render_metadata.screen_width_in_tiles = 90;// m_tilemap->width_in_tiles();
			render_metadata.inv_tile_screen_width = invTileSize.x;
			render_metadata.inv_tile_screen_height = invTileSize.y;

			if (m_tile_render_info == nullptr)
			{
				m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(TilemapRenderingMetaData));
			}

			renderCtx->update_buffer(m_tile_render_info.get(), &render_metadata, sizeof(render_metadata));
		}
		void BlockRenderPass::init_tile_indices_uab(rex::gfx::RenderContext* renderCtx)
		{
			if (!m_tiles_indices_buffer || m_tiles_indices_buffer->size() < m_tilemap->num_tiles())
			{
				m_tiles_indices_buffer = rex::gfx::gal::instance()->create_unordered_access_buffer(m_tilemap->num_tiles());
			}
		}
		void BlockRenderPass::init_render_pass()
		{
			m_render_pass_desc.name = "Tile Renderer";

			m_render_pass_desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

			// We're rendering directly to the back buffer
			m_render_pass_desc.framebuffer_desc.clear();
			m_render_pass_desc.framebuffer_desc.emplace_back(rex::gfx::swapchain_frame_buffer_handle());

			// Assign the shaders used for the tile renderer
			m_render_pass_desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
			m_render_pass_desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

			m_render_pass_desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			m_render_pass = rsl::make_unique<rex::gfx::RenderPass>(m_render_pass_desc);
			m_render_pass->set("tile_texture", m_tileset->tileset_texture()->texture_resource());
			m_render_pass->set("default_sampler", default_sampler);
			m_render_pass->set("RenderingMetaData", m_tile_render_info.get());
			m_render_pass->set("TileIndexIntoTextureBuffer", m_tiles_indices_buffer.get());

		}

		void BlockRenderPass::update_tilemap(const BlockRenderPassParams& params)
		{
			if (m_tilemap == nullptr || m_tilemap->width_in_blocks() != params.screen_resolution.x || m_tilemap->height_in_blocks() != params.screen_resolution.y)
			{
				m_tilemap = rsl::make_unique<rex::Tilemap>(params.screen_resolution.x / 4, params.screen_resolution.y / 4);
			}

			init();

			s32 num_tiles_until_end_of_row = params.world_width_in_tiles - params.top_left_start.x;
			s32 num_to_copy = rsl::min(params.screen_resolution.x, num_tiles_until_end_of_row);

			s32 start_idx = params.top_left_start.y * params.world_width_in_tiles + params.top_left_start.x;
			const u8* src = params.tiles_source + start_idx;
			s32 offset = 0;
			for (s32 row = 0; row < params.screen_resolution.y / 4; ++row)
			{
				m_tilemap->set(src, num_to_copy, offset);
				offset += params.screen_resolution.x;
				src += params.world_width_in_tiles;
			}
		}

	}
}