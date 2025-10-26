#include "rex_engine/gfx/rendering/render_passes/block_pass.h"

#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/rendering/tile_vertex.h"

#include "rex_engine/filesystem/path.h"

namespace rex
{
	namespace gfx
	{
		BlockRenderPass::BlockRenderPass(const BlockRenderPassCreationInfo& creationInfo)
			: RenderPass(create_desc())
			, m_render_target(creationInfo.render_target)
			, m_scene_params()
			, m_camera_params()
			, m_render_metadata()
		{
			// The following member variables are allowed to be null by the endo of construction
			// tileset: The tileset is set at runtime as it depends on which map is currently loaded
			// tilemap: Can be null as the user should set the screen resolution. This screen resolution is used to initialize the tilemap
			// tile render info: The tileset needs to be set in order to have this info. The tileset is set at runtime

			//update_dynamic_inputs(inputs);
			init();
		}

		void BlockRenderPass::update_scene_params(const SceneParams& params)
		{
			m_scene_params = params;

			set("tile_texture", m_scene_params.tileset->tileset_texture());

			auto render_ctx = gal::instance()->new_render_ctx();

			init_tilemap();
			init_render_info(render_ctx.get());
			init_tile_indices_uab(render_ctx.get());
		}
		void BlockRenderPass::update_camera_params(const CameraParams& params)
		{
			m_camera_params = params;

			auto render_ctx = gal::instance()->new_render_ctx();
			init_tilemap();
			init_render_info(render_ctx.get());
			init_tile_indices_uab(render_ctx.get());
		}

		//void BlockRenderPass::update_dynamic_inputs(const BlockRenderPassDynamicInputs& inputs)
		//{
		//	if (inputs.tileset != nullptr)
		//	{
		//		m_scene_params.tileset = inputs.tileset;
		//		set("tile_texture", m_scene_params.tileset->tileset_texture());
		//	}
		//	if (inputs.render_target != nullptr)
		//	{
		//		m_render_target = inputs.render_target;
		//	}
		//	if (inputs.screen_resolution.x.get() != 0 && inputs.screen_resolution.y.get() != 0)
		//	{
		//		m_screen_resolution = inputs.screen_resolution;
		//		init_tilemap();
		//	}

		//	auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		//	init_render_info(render_ctx.get());
		//}

		void BlockRenderPass::update_tilemap(const BlockRenderPassTilemapParams& params)
		{
			if (!m_tilemap)
			{
				return;
			}

			TileCoord tile_coords = params.coord_converter.to_tile_coord(params.top_left_start);

			m_render_metadata.screen_pixel_offset_x = -params.top_left_start.x % params.coord_converter.num_pixels_per_tile;
			m_render_metadata.screen_pixel_offset_y = params.top_left_start.y % params.coord_converter.num_pixels_per_tile;

			auto render_ctx = gal::instance()->new_render_ctx();
			render_ctx->update_buffer(m_tile_render_info.get(), &m_render_metadata, sizeof(m_render_metadata));

			rsl::point<TileCount> screen_resolution = calc_screen_resolution();
			s32 num_tiles_until_end_of_row = params.world_width_in_tiles - tile_coords.x;
			s32 num_to_copy = rsl::min(screen_resolution.x.get(), num_tiles_until_end_of_row);

			s32 start_idx = tile_coords.y * params.world_width_in_tiles + tile_coords.x;
			const u8* src = params.tiles_source + start_idx;
			s32 offset = 0;
			for (s32 row = 0; row < screen_resolution.y.get(); ++row)
			{
				m_tilemap->set(src, num_to_copy, offset);
				offset += screen_resolution.x.get();
				src += params.world_width_in_tiles;
			}
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

			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());

			// Send the draw command
			const s32 index_count_per_instance = 6;
			const s32 instance_count = m_tilemap->num_tiles();
			renderCtx->draw_indexed_instanced(index_count_per_instance, instance_count, 0, 0, 0);
		}

		void BlockRenderPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");
			//REX_ASSERT_X(m_scene_params.tileset != nullptr, "No tileset provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			//init_render_info(render_ctx.get());
			//init_tilemap();
			init_shader_params();
		}

		void BlockRenderPass::init_vb(rex::gfx::RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};

			tile_vertices[0] = TileVertex{ rsl::point<f32>(0,  0),               rsl::point<f32>(0.0f, 0.0f) };
			tile_vertices[1] = TileVertex{ rsl::point<f32>(1,	 0),               rsl::point<f32>(1.0f, 0.0f) };
			tile_vertices[2] = TileVertex{ rsl::point<f32>(0,	-1),							 rsl::point<f32>(0.0f, 1.0f) };
			tile_vertices[3] = TileVertex{ rsl::point<f32>(1,	-1),							 rsl::point<f32>(1.0f, 1.0f) };

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
		void BlockRenderPass::init_render_info(rex::gfx::RenderContext* renderCtx)
		{
			if (!m_scene_params.tileset)
			{
				return;
			}

			rsl::pointi8 tile_size = m_scene_params.tileset->tile_size();
			rsl::point<f32> inv_zoom_level{};
			inv_zoom_level.x = 1.0f / m_camera_params.zoom_level.x;
			inv_zoom_level.y = 1.0f / m_camera_params.zoom_level.y;

			// inverse tile width comes from diving from 2
			// this is because ndc coordinates have a width of 2 (going from -1 to 1)

			rsl::pointi8 num_tiles_on_screen{};
			//num_tiles_on_screen.x = tile_size.x;
			//num_tiles_on_screen.y = tile_size.y;

			//num_tiles_on_screen.x = m_render_target->width() / tile_size.x;
			//num_tiles_on_screen.y = m_render_target->height() / tile_size.y;

			//num_tiles_on_screen.x = m_render_target->width() / (tile_size.x * inv_zoom_level.x);
			//num_tiles_on_screen.y = m_render_target->height() / (tile_size.y * inv_zoom_level.y);

			num_tiles_on_screen.x = m_render_target->width() / (tile_size.x * m_camera_params.zoom_level.x);
			num_tiles_on_screen.y = m_render_target->height() / (tile_size.y * m_camera_params.zoom_level.y);

			f32 inv_tile_width = 2.0f / num_tiles_on_screen.x;
			f32 inv_tile_height = 2.0f / num_tiles_on_screen.y;

			s32 tileset_width = m_scene_params.tileset->tileset_texture()->width();
			s32 tileset_height = m_scene_params.tileset->tileset_texture()->height();

			rsl::vec2 uv_size{};
			uv_size.x = tile_size.x / (f32)tileset_width;
			uv_size.y = tile_size.y / (f32)tileset_height;

			// Init the constant buffer

			m_render_metadata.texture_tiles_per_row = m_scene_params.tileset->num_tiles_per_row();
			m_render_metadata.inv_texture_width = uv_size.x;
			m_render_metadata.inv_texture_height = uv_size.y;

			m_render_metadata.screen_width_in_tiles = m_tilemap->width().get();
			m_render_metadata.inv_tile_screen_width = inv_tile_width;
			m_render_metadata.inv_tile_screen_height = inv_tile_height;

			m_render_metadata.screen_pixel_offset_x = 0;
			m_render_metadata.screen_pixel_offset_y = 0;
			//m_render_metadata.inv_pixel_screen_width = 1.0f / (2.0f * tile_size.x * m_camera_params.zoom_level.x); // how big 
			m_render_metadata.inv_pixel_screen_width = m_camera_params.zoom_level.x / m_render_target->width(); // how big is 1 tile pixel on the screen
			m_render_metadata.inv_pixel_screen_height = m_camera_params.zoom_level.y / m_render_target->height(); // how big is 1 tile pixel on the screen

			if (m_tile_render_info == nullptr)
			{
				m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(TilemapRenderingMetaData));
				set("RenderingMetaData", m_tile_render_info.get());
			}

			renderCtx->update_buffer(m_tile_render_info.get(), &m_render_metadata, sizeof(m_render_metadata));
		}
		void BlockRenderPass::init_tile_indices_uab(rex::gfx::RenderContext* renderCtx)
		{
			if (!m_tiles_indices_buffer || m_tiles_indices_buffer->size() < m_tilemap->num_tiles())
			{
				m_tiles_indices_buffer = rex::gfx::gal::instance()->create_unordered_access_buffer(m_tilemap->num_tiles());
				set("TileIndexIntoTextureBuffer", m_tiles_indices_buffer.get());
			}
		}
		RenderPassDesc BlockRenderPass::create_desc() const
		{
			RenderPassDesc desc{};
			desc.name = "Block Render Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

			// We're rendering directly to the back buffer
			desc.framebuffer_desc.clear();
			desc.framebuffer_desc.emplace_back(rex::gfx::swapchain_frame_buffer_handle());

			// Assign the shaders used for the tile renderer
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(rex::engine::instance()->project_root(), "shaders", "render_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			return desc;
		}
		void BlockRenderPass::init_shader_params()
		{
			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			set("default_sampler", default_sampler);
		}
		void BlockRenderPass::init_tilemap()
		{
			REX_ASSERT_X(m_render_target != nullptr, "Cannot create a tilemap without a render target. The render target is needed to calculate how many tiles we can fit on screen");
			REX_ASSERT_X(m_scene_params.tileset != nullptr, "Cannot create a tilemap without a tileset as we need the size of a single tile in pixels");

			rsl::point<TileCount> screen_resolution = calc_screen_resolution();

			// Not sure if we should calculate the number of tiles to render or have the user specify this
			// The gameboy has a set resolution, which is the tile resolution we should target for the game
			m_tilemap = rsl::make_unique<rex::Tilemap>(screen_resolution);
		}

		rsl::point<TileCount> BlockRenderPass::calc_screen_resolution() const
		{
			rsl::point<TileCount> screen_resolution{};
			screen_resolution.x.get() = m_render_target->width() / (m_scene_params.tileset->tile_size().x * m_camera_params.zoom_level.x);
			screen_resolution.y.get() = m_render_target->height() / (m_scene_params.tileset->tile_size().y * m_camera_params.zoom_level.y);

			// Draw 1 extra tile on each axis to allow for panning
			// This is so there won't be a black border drawn
			// when the camera is moving
			screen_resolution.x.get() += 1;
			screen_resolution.y.get() += 1;

			return screen_resolution;
		}

	}
}