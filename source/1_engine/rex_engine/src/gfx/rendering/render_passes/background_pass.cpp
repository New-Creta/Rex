#include "rex_engine/gfx/rendering/render_passes/background_pass.h"

#include "rex_engine/gfx/core/vertex.h"
#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/system/resource_manager.h"

#include "rex_engine/filesystem/path.h"

namespace rex
{
	namespace gfx
	{
		using TileVertex = VertexPos2DTex;

		BlockRenderPass::BlockRenderPass(const BlockRenderPassCreationInfo& creationInfo)
			: RenderPass(create_desc(creationInfo))
			, m_render_target(creationInfo.render_target)
			, m_render_metadata()
		{
			// The following member variables are allowed to be null by the endo of construction
			// tileset: The tileset is set at runtime as it depends on which map is currently loaded
			// tilemap: Can be null as the user should set the screen resolution. This screen resolution is used to initialize the tilemap
			// tile render info: The tileset needs to be set in order to have this info. The tileset is set at runtime

			init();
		}

		void BlockRenderPass::update_params(const SceneRenderParams& params)
		{
			if (!params.tileset)
			{
				return;
			}

			auto render_ctx = gal::instance()->new_render_ctx();

			update_tilemaps(render_ctx.get(), params);
			update_render_metadata(render_ctx.get(), params);
			set("tile_texture", params.tileset->tileset_texture());
		}

		void BlockRenderPass::render(rex::gfx::RenderContext* renderCtx)
		{
			if (!m_screen_tilemap_cpu)
			{
				return;
			}

			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());

			// Send the draw command
			const s32 index_count_per_instance = 6;
			const s32 instance_count = m_screen_tilemap_cpu->num_tiles();
			renderCtx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
		}

		void BlockRenderPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_shader_params();
			init_tile_render_info();
		}
		void BlockRenderPass::init_vb(rex::gfx::RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};

			tile_vertices[0] = TileVertex{ glm::vec2(0,  0),    glm::vec2(0.0f, 0.0f) };
			tile_vertices[1] = TileVertex{ glm::vec2(1,	 0),    glm::vec2(1.0f, 0.0f) };
			tile_vertices[2] = TileVertex{ glm::vec2(0,	-1),	glm::vec2(0.0f, 1.0f) };
			tile_vertices[3] = TileVertex{ glm::vec2(1,	-1),	glm::vec2(1.0f, 1.0f) };

			m_tiles_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(TileVertex));

			renderCtx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(TileVertex));
			renderCtx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void BlockRenderPass::init_ib(rex::gfx::RenderContext* renderCtx)
		{
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
		void BlockRenderPass::init_shader_params()
		{
			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			set("default_sampler", default_sampler);
		}
		void BlockRenderPass::init_tile_render_info()
		{
			if (m_tile_render_info == nullptr)
			{
				m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(TilemapRenderingMetaData));
				set("RenderingMetaData", m_tile_render_info.get());
			}
		}

		RenderPassDesc BlockRenderPass::create_desc(const BlockRenderPassCreationInfo& creationInfo) const
		{
			RenderPassDesc desc{};
			desc.name = "Background Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

			desc.framebuffer_desc.emplace_back(creationInfo.render_target);

			// Assign the shaders used for the tile renderer
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->engine_root(), "shaders", "background.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->engine_root(), "shaders", "background.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout = TileVertex::layout();

			return desc;
		}
		
		void BlockRenderPass::update_tilemaps(rex::gfx::RenderContext* renderCtx, const SceneRenderParams& params)
		{
			REX_ASSERT_X(m_render_target != nullptr, "Cannot create a tilemap without a render target. The render target is needed to calculate how many tiles we can fit on screen");
			REX_ASSERT_X(params.tileset != nullptr, "Cannot create a tilemap without a tileset as we need the size of a single tile in pixels");
			REX_ASSERT_X(params.camera != nullptr, "Cannot create a tilemap without a camera as we need the zoom factor");

			rsl::point<TileCount> screen_resolution = calc_screen_resolution(params.camera, params.tileset);

			// Create the object if they don't exist or if they require an update in dimensions
			if (!m_screen_tilemap_cpu || screen_resolution.x.get() * screen_resolution.y.get() != m_screen_tilemap_cpu->num_tiles())
			{
				m_screen_tilemap_cpu = rsl::make_unique<rex::Tilemap>(screen_resolution);
			}
			if (!m_screen_tilemap_gpu || m_screen_tilemap_gpu->size() < m_screen_tilemap_cpu->num_tiles())
			{
				m_screen_tilemap_gpu = rex::gfx::gal::instance()->create_unordered_access_buffer(m_screen_tilemap_cpu->num_tiles());
				set("TileIndexIntoTextureBuffer", m_screen_tilemap_gpu.get());
			}

			// fill in the data and upload it to the gpu
			TileCoord tile_coords = params.coord_converter.to_tile_coord(params.camera->top_left());
			s32 num_tiles_until_end_of_row = params.world_width_in_tiles.get() - tile_coords.x;
			s32 num_to_copy = rsl::min(screen_resolution.x.get(), num_tiles_until_end_of_row);

			s32 start_idx = tile_coords.y * params.world_width_in_tiles.get() + tile_coords.x;
			const u8* src = params.tiles_source + start_idx;
			s32 offset = 0;
			for (s32 row = 0; row < screen_resolution.y.get(); ++row)
			{
				m_screen_tilemap_cpu->set(src, num_to_copy, offset);
				offset += screen_resolution.x.get();
				src += params.world_width_in_tiles.get();
			}

			const s32 tile_byte_size = sizeof(m_screen_tilemap_cpu->tiles()[0]);
			renderCtx->update_buffer(m_screen_tilemap_gpu.get(), m_screen_tilemap_cpu->tiles(), m_screen_tilemap_cpu->num_tiles() * tile_byte_size);
			renderCtx->transition_buffer(m_screen_tilemap_gpu.get(), rex::gfx::ResourceState::NonPixelShaderResource);
		}
		void BlockRenderPass::update_render_metadata(rex::gfx::RenderContext* renderCtx, const SceneRenderParams& params)
		{
			// inverse tile width comes from diving from 2
			// this is because ndc coordinates have a width of 2 (going from -1 to 1)
			rsl::pointi8 tile_size = params.tileset->tile_size();

			s32 tileset_width = params.tileset->tileset_texture()->width();
			s32 tileset_height = params.tileset->tileset_texture()->height();

			// ---------------
			// TEXTURE INFO
			// ---------------
			rsl::vec2 uv_size{};
			uv_size.x = tile_size.x / (f32)tileset_width;
			uv_size.y = tile_size.y / (f32)tileset_height;

			m_render_metadata.texture_tiles_per_row = params.tileset->num_tiles_per_row();
			m_render_metadata.inv_texture_width = uv_size.x;
			m_render_metadata.inv_texture_height = uv_size.y;

			// ---------------
			// TILE INFO
			// ---------------
			rsl::pointi8 num_tiles_on_screen{};
			num_tiles_on_screen.x = narrow_cast<s8>(m_render_target->width() / (tile_size.x * params.camera->zoom().x));
			num_tiles_on_screen.y = narrow_cast<s8>(m_render_target->height() / (tile_size.y * params.camera->zoom().y));

			f32 inv_tile_width = 2.0f / num_tiles_on_screen.x;
			f32 inv_tile_height = 2.0f / num_tiles_on_screen.y;
			m_render_metadata.screen_width_in_tiles = m_screen_tilemap_cpu->width().get();
			m_render_metadata.inv_tile_screen_width = inv_tile_width;
			m_render_metadata.inv_tile_screen_height = inv_tile_height;

			// ---------------
			// SCREEN INFO
			// ---------------
			PixelCoord top_left = params.camera->top_left();
			s32 x_mod = top_left.x % params.coord_converter.num_pixels_per_tile;
			s32 y_mod = top_left.y % params.coord_converter.num_pixels_per_tile;
			m_render_metadata.screen_pixel_offset_x = -x_mod;
			m_render_metadata.screen_pixel_offset_y = y_mod;

			m_render_metadata.inv_pixel_screen_width = 2 * params.camera->zoom().x / m_render_target->width(); // how big is 1 tile pixel on the screen
			m_render_metadata.inv_pixel_screen_height = 2 * params.camera->zoom().y / m_render_target->height(); // how big is 1 tile pixel on the screen

			renderCtx->update_buffer(m_tile_render_info.get(), &m_render_metadata, sizeof(m_render_metadata));
		}

		rsl::point<TileCount> BlockRenderPass::calc_screen_resolution(const Camera2D* camera, const TilesetAsset* tileset) const
		{
			rsl::point<TileCount> screen_resolution{};
			screen_resolution.x.get() = static_cast<s32>(m_render_target->width() / (tileset->tile_size().x * camera->zoom().x));
			screen_resolution.y.get() = static_cast<s32>(m_render_target->height() / (tileset->tile_size().y * camera->zoom().y));

			// Draw 1 extra tile on each axis to allow for panning
			// This is so there won't be a black border drawn
			// when the camera is moving
			screen_resolution.x.get() += 1;
			screen_resolution.y.get() += 1;

			return screen_resolution;
		}

	}
}