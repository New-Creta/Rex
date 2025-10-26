#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "rex_engine/filesystem/path.h"

#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/rendering/tile_vertex.h"

#include "rex_engine/gfx/resources/animated_sprite.h"

// rendering animated data is pretty straightforward
// use the same principles as rendering the static tiles
// however we also have to store where we want to draw the tile
// as we can't assume it from the instance ID
// this location will be stored as a index and we can calculate
// the final position based on the inv tile size on the render target
// It's more likely that the texture will change in a given pass,
// so we also have to make sure we're using the correct texture for rendering
// this can be done by storing an array of textures in the shader instead of a single one
// and the per instance data can also store which texture idx need to be used

namespace rex
{
	namespace gfx
	{
		struct SceneRenderInfo
		{
			f32 inv_tile_screen_width;
			f32 inv_tile_screen_height;
			//};
			//struct InstanceRenderInfo
			//{
			f32 inv_sprite_screen_width;
			f32 inv_sprite_screen_height;

			f32 inv_sprite_texture_width;
			f32 inv_sprite_texture_height;
		};

		AnimatedSpritesPass::AnimatedSpritesPass(const AnimatedSpritePassCreationInfo& creationInfo)
			: RenderPass(create_desc())
			, m_render_target(creationInfo.render_target)
		{
			init();
		}

		void AnimatedSpritesPass::update_scene_params(const SceneParams& params)
		{
			m_scene_params = params;
		}
		void AnimatedSpritesPass::update_camera_params(const CameraParams& params)
		{
			m_camera_params = params;

			auto render_ctx = gal::instance()->new_render_ctx();

			init_render_info(render_ctx.get());
		}

		AnimatedSprite* AnimatedSpritesPass::add_sprite(rsl::unique_ptr<AnimatedSprite> sprite)
		{
			m_sprites.emplace_back(rsl::move(sprite));
			return m_sprites.back().get();
		}
		void AnimatedSpritesPass::push_sprite(const AnimatedSpriteDrawList& drawlist)
		{
			s32 max_num_drawlists = 16; // the maximum number of animated characters on screen in pokemon
			REX_ASSERT_X(m_draw_list.size() < max_num_drawlists, "Cannot push more than {} animated sprites into the drawlist queue", max_num_drawlists);

			m_draw_list.push_back(drawlist);
		}

		void AnimatedSpritesPass::render(rex::gfx::RenderContext* renderCtx)
		{
			//if (m_draw_list.empty())
			//{
			//	return;
			//}
		
			bind_to(renderCtx);

			// Bind all the resources to the gfx pipeline
			renderCtx->set_vertex_buffer(m_tiles_vb_gpu.get(), 0);
			renderCtx->set_index_buffer(m_tiles_ib_gpu.get());
			//renderCtx->set_render_target(m_render_target);
			//renderCtx->clear_render_target(m_render_target);

			//s32 render_target_width = m_render_target->width();
			//s32 render_target_height = m_render_target->height();

			//f32 viewport_width = static_cast<f32>(render_target_width);
			//f32 viewport_height = static_cast<f32>(render_target_height);
			//rex::gfx::Viewport viewport = { glm::vec2(0.0f, 0.0f), glm::vec2(viewport_width, viewport_height), 0.0f, 1.0f };
			//renderCtx->set_viewport(viewport);

			//rex::gfx::ScissorRect rect = { 0, 0, viewport_width, viewport_height };
			//renderCtx->set_scissor_rect(rect);

			struct PerInstanceCB
			{
				u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
				f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
				f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

				s16 sprite_idx;
				bool flip_x;
				bool flip_y;
			};

			s32 index_count = m_tiles_ib_gpu->count();
			for (const rsl::unique_ptr<AnimatedSprite>& sprite : m_sprites)
			{
				SceneRenderInfo scene_render_info{};

				scene_render_info.inv_sprite_texture_width = 1.0f / sprite->sprites_texture()->width();
				scene_render_info.inv_sprite_texture_height = 1.0f / sprite->sprites_texture()->height();

				rsl::pointi8 tile_size = m_scene_params.tileset->tile_size();
				rsl::pointi8 sprite_size = sprite->sprite_size();
				rsl::point<f32> inv_zoom_level{};
				inv_zoom_level.x = 1.0f / m_camera_params.zoom_level.x;
				inv_zoom_level.y = 1.0f / m_camera_params.zoom_level.y;

				rsl::pointi8 num_tiles_on_screen{};
				rsl::pointi8 num_sprites_on_screen{};

				num_tiles_on_screen.x = m_render_target->width() / (tile_size.x * m_camera_params.zoom_level.x);
				num_tiles_on_screen.y = m_render_target->height() / (tile_size.y * m_camera_params.zoom_level.y);

				num_sprites_on_screen.x = m_render_target->width() / (sprite_size.x * m_camera_params.zoom_level.x);
				num_sprites_on_screen.y = m_render_target->height() / (sprite_size.y * m_camera_params.zoom_level.y);

				f32 inv_tile_width = 2.0f / num_tiles_on_screen.x;
				f32 inv_tile_height = 2.0f / num_tiles_on_screen.y;

				f32 inv_sprite_width = 2.0f / num_sprites_on_screen.x;
				f32 inv_sprite_height = 2.0f / num_sprites_on_screen.y;

				scene_render_info.inv_sprite_screen_width = inv_sprite_width;
				scene_render_info.inv_sprite_screen_height = inv_sprite_height;

				scene_render_info.inv_tile_screen_width = inv_tile_width;
				scene_render_info.inv_tile_screen_height = inv_tile_height;

				renderCtx->update_buffer(m_tile_render_info.get(), &scene_render_info, sizeof(scene_render_info));

				set("sprite_texture", sprite->sprites_texture());

				renderCtx->draw_indexed(index_count, 0, 0, 0);

				// this loop should essentially be the bottom loop
				// when the user creates an animated sprite texture
				// they can add that to the renderer
				
				// the renderer will then add it to correct pass (eg this one)
				// which will then draw it to the screen
			}

			//for (const AnimatedSpriteDrawList& drawlist : m_draw_list)
			//{
			//	rsl::vec2 uv_size{};
			//	rsl::pointi8 tile_size = drawlist.tileset->tile_size();
			//	uv_size.x = tile_size.x / (f32)drawlist.tileset->tileset_texture()->width();
			//	uv_size.y = tile_size.y / (f32)drawlist.tileset->tileset_texture()->height();

			//	PerInstanceCB per_instance_cb{ };
			//	per_instance_cb.texture_tiles_per_row = drawlist.tileset->num_tiles_per_row();
			//	per_instance_cb.inv_texture_width = uv_size.x;
			//	per_instance_cb.inv_texture_width = uv_size.y;

			//	per_instance_cb.sprite_idx = drawlist.sprite_idx;
			//	per_instance_cb.flip_x = drawlist.flip_x;
			//	per_instance_cb.flip_y = drawlist.flip_y;

			//	set("sprite_texture", drawlist.tileset->tileset_texture());

			//	renderCtx->update_buffer(m_per_instance_cb.get(), &per_instance_cb, sizeof(per_instance_cb));
			//	renderCtx->transition_buffer(m_per_instance_cb.get(), rex::gfx::ResourceState::NonPixelShaderResource);

			//	const s32 index_count_per_instance = 6;
			//	renderCtx->draw_indexed_instanced(index_count_per_instance, m_draw_list.size(), 0, 0, 0);
			//}

			m_draw_list.clear();
		}

		void AnimatedSpritesPass::init()
		{
			REX_ASSERT_X(m_render_target != nullptr, "No render target provided for render pass");

			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();

			init_vb(render_ctx.get());
			init_ib(render_ctx.get());
			init_shader_params();
		}

		void AnimatedSpritesPass::init_vb(rex::gfx::RenderContext* renderCtx)
		{
			// Init the vertex buffer
			const s32 num_vertices_per_tile = 4;

			rsl::array<TileVertex, num_vertices_per_tile> tile_vertices{};

			tile_vertices[0] = TileVertex{ rsl::point<f32>(0,  0),               rsl::point<f32>(0.0f, 0.0f) };
			tile_vertices[1] = TileVertex{ rsl::point<f32>(1,	 0),               rsl::point<f32>(1.0f, 0.0f) };
			tile_vertices[2] = TileVertex{ rsl::point<f32>(0,	-1),							 rsl::point<f32>(0.0f, 1.0f) };
			tile_vertices[3] = TileVertex{ rsl::point<f32>(1,	-1),							 rsl::point<f32>(1.0f, 1.0f) };

			if (!m_tiles_vb_gpu)
			{
				m_tiles_vb_gpu = rex::gfx::gal::instance()->create_vertex_buffer(num_vertices_per_tile, sizeof(TileVertex));
			}

			renderCtx->update_buffer(m_tiles_vb_gpu.get(), tile_vertices.data(), tile_vertices.size() * sizeof(TileVertex));
			renderCtx->transition_buffer(m_tiles_vb_gpu.get(), rex::gfx::ResourceState::VertexAndConstantBuffer);
		}
		void AnimatedSpritesPass::init_ib(rex::gfx::RenderContext* renderCtx)
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
		void AnimatedSpritesPass::init_render_info(rex::gfx::RenderContext* renderCtx)
		{
			// inverse tile width comes from diving from 2
			// this is because ndc coordinates have a width of 2 (going from -1 to 1)
			//f32 inv_tile_width = 2.0f / m_screen_resolution.x.get();
			//f32 inv_tile_height = 2.0f / m_screen_resolution.y.get();




			//// Init the constant buffer
			//struct TilemapRenderingMetaData
			//{
			//	// Render target data
			//	u32 screen_width_in_tiles;   // the number of tiles we render on a single row
			//	f32 inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
			//	f32 inv_tile_screen_height;  // the inverse of the height of a single tile on the screen
			//};

			//TilemapRenderingMetaData render_metadata{};

			//render_metadata.screen_width_in_tiles = m_screen_resolution.x.get();
			//render_metadata.inv_tile_screen_width = inv_tile_width;
			//render_metadata.inv_tile_screen_height = inv_tile_height;

			if (m_tile_render_info == nullptr)
			{
				m_tile_render_info = rex::gfx::gal::instance()->create_constant_buffer(sizeof(SceneRenderInfo));
				set("RenderingMetaData", m_tile_render_info.get());
			}
		}
		void AnimatedSpritesPass::init_tile_indices_uab()
		{
			s32 num_allowed_animated_characters = 16;
			m_per_instance_info = rex::gfx::gal::instance()->create_unordered_access_buffer(sizeof(PerSpriteInstanceData) * num_allowed_animated_characters);
		}
		void AnimatedSpritesPass::init_shader_params()
		{
			rex::gfx::Sampler2D* default_sampler = rex::gfx::gal::instance()->common_sampler(rex::gfx::CommonSampler::Default2D);

			set("default_sampler", default_sampler);
			//set("TileIndexIntoTextureBuffer", m_per_instance_info.get());
		}

		RenderPassDesc AnimatedSpritesPass::create_desc() const
		{
			RenderPassDesc desc{};

			desc.name = "Animated Sprites Pass";

			desc.pso_desc.output_merger.raster_state = rex::gfx::gal::instance()->common_raster_state(rex::gfx::CommonRasterState::DefaultDepth);

			// We're rendering directly to the back buffer
			desc.framebuffer_desc.clear();
			desc.framebuffer_desc.emplace_back(rex::gfx::swapchain_frame_buffer_handle());

			// Assign the shaders used for the tile renderer
			rex::scratch_string project_shaders = rex::path::join(rex::engine::instance()->project_root(), "shaders");
			desc.pso_desc.shader_pipeline.vs = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "render_animated_tile_vertex.hlsl"), rex::gfx::ShaderType::Vertex);
			desc.pso_desc.shader_pipeline.ps = rex::gfx::shader_lib::instance()->load(rex::path::join(project_shaders, "render_animated_tile_pixel.hlsl"), rex::gfx::ShaderType::Pixel);

			desc.pso_desc.input_layout =
			{
				// Per vertex data
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::Position, rex::gfx::ShaderArithmeticType::Float2 },
				rex::gfx::InputLayoutElementDesc{ rex::gfx::ShaderSemantic::TexCoord, rex::gfx::ShaderArithmeticType::Float2 },
			};

			return desc;
		}
	}
}