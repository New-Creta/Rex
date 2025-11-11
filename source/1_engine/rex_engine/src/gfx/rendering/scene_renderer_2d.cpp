#include "rex_engine/gfx/rendering/scene_renderer_2d.h"

#include "rex_engine/gfx/system/resource_manager.h"

namespace rex
{
	namespace gfx
	{
		SceneRenderer2D::SceneRenderer2D()
			: m_zoom_level(1.0f, 1.0f)
		{

			m_render_target = rex::gfx::gal::instance()->create_render_target(
				gal::instance()->back_buffer_width(),
				gal::instance()->back_buffer_height(),
				TextureFormat::Unorm4Srgb
				);
			//BackBufferRenderTarget* render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();

			ClearStateDesc clear_state{};
			clear_state.flags.add_state(ClearBits::ClearStencilBuffer);
			clear_state.stencil = 0;
			m_stencil_buffer = rex::gfx::gal::instance()->create_depth_stencil_buffer(m_render_target->width(), m_render_target->height(), TextureFormat::Depth24Stencil8, clear_state);
			resource_manager::instance()->add_depth_stencil_buffer(m_stencil_buffer.get(), "World Stencil Buffer");

			//rsl::point<TileCount> screen_resolution{};
			//screen_resolution.x.get() = render_target->width() / m_zoom_level;
			//screen_resolution.y.get() = render_target->height() / m_zoom_level;

			// draw pass to render the static tiles that don't change
			// this is essentially just the background
			//rex::gfx::BlockRenderPassDynamicInputs block_pass_inputs{};
			//block_pass_inputs.render_target = render_target;
			//block_pass_inputs.screen_resolution = screen_resolution;
			//block_pass_inputs.tileset = tileset;
			//block_pass_inputs.screen_resolutions = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };
			BlockRenderPassCreationInfo blockpass_creation_info{};
			blockpass_creation_info.render_target = m_render_target.get();

			m_block_render_pass = rsl::make_unique<BlockRenderPass>(blockpass_creation_info);

			// draw the animated tiles
			// the player, NPCs, flowers, ...
			// sprite movement in old games is found in movement.asm
			//rex::gfx::AnimatedSpritesPassDynamicInputs animated_sprites_inputs{};
			//animated_sprites_inputs.render_target = render_target;
			//animated_sprites_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };
			AnimatedSpritePassCreationInfo animatedspritepass_creation_info{};
			animatedspritepass_creation_info.render_target = m_render_target.get();
			m_animated_sprites_pass = rsl::make_unique<rex::gfx::AnimatedSpritesPass>(animatedspritepass_creation_info);

			// draw the water
			// water is animated by bit shifting its pixels
			//m_water_pass = rex::gfx::renderer::instance()->add_pass(rsl::make_unique<rex::gfx::BlockRenderPass>(inputs));

			// draw the UI
			//m_ui_pass = rex::gfx::renderer::instance()->add_render_pass(rsl::make_unique<UiPass>());

			// composite the final render target based on input of above
			 CompositePassCreationInfo compositepass_creation_info{};
			 compositepass_creation_info.dst_render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
			 compositepass_creation_info.src_render_target = m_render_target.get();
			 m_composite_pass = rsl::make_unique<CompositePass>(compositepass_creation_info);
		}

		void SceneRenderer2D::render()
		{
			auto render_ctx = gal::instance()->new_render_ctx();

			BackBufferRenderTarget* render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();

			render_ctx->set_render_target(render_target, m_stencil_buffer.get());
			render_ctx->clear_render_target(render_target, m_stencil_buffer.get());

			f32 render_target_width = render_target->width();
			f32 render_target_height = render_target->height();

			rex::gfx::Viewport viewport = { glm::vec2(0.0f, 0.0f), glm::vec2(render_target_width, render_target_height), 0.0f, 1.0f };
			render_ctx->set_viewport(viewport);

			rex::gfx::ScissorRect rect = { 0, 0, render_target_width, render_target_height };
			render_ctx->set_scissor_rect(rect);

			render_tilemap(render_ctx.get());
			render_flipbook_animations(render_ctx.get());
			render_dynamic_animations(render_ctx.get());

			m_composite_pass->render(render_ctx.get());
		}

		void SceneRenderer2D::add_animated_sprite(rsl::unique_ptr<AnimatedSprite> animatedSprite)
		{
			m_animated_sprites_pass->add_sprite(rsl::move(animatedSprite));
		}

		void SceneRenderer2D::update_params(const SceneRenderParams& params)
		{
			m_params = params;
		}

		void SceneRenderer2D::update_zoom(rsl::point<f32> zoomLevel)
		{
			m_zoom_level = zoomLevel;

			// The zoom indicates how many tiles we can render
			// If zoom is 1, that means we can render
			// render target width / tile size to the screen
			// if we have a zoom value, we change this default
			// to draw more or less tiles to the screen
			CameraParams params{};
			params.zoom_level = m_zoom_level;

			m_block_render_pass->update_camera_params(params);
			m_animated_sprites_pass->update_camera_params(params);
		}

		void SceneRenderer2D::notify_new_tileset(const TilesetAsset* tileset)
		{
			SceneParams params{};
			params.tileset = tileset;

			m_block_render_pass->update_scene_params(params);
			m_animated_sprites_pass->update_scene_params(params);

			//BlockRenderPassDynamicInputs inputs{};
			//inputs.tileset = tileset;

			//m_block_render_pass->update_dynamic_inputs(inputs);
		}

		void SceneRenderer2D::render_tilemap(RenderContext* renderCtx)
		{
			BlockRenderPassTilemapParams params{};
			params.tiles_source = m_params.tiles_source;//  m_scene_tilemap->tiles();
			params.top_left_start = m_params.top_left;// m_top_left;
			params.coord_converter = m_params.coord_converter;
			params.world_width_in_tiles = m_params.world_width_in_tiles.get();// m_scene_tilemap->width().get();

			m_block_render_pass->update_tilemap(params);
			m_block_render_pass->render(renderCtx);
		}

		void SceneRenderer2D::render_flipbook_animations(RenderContext* renderCtx)
		{
			m_animated_sprites_pass->render(renderCtx);
		}

		void SceneRenderer2D::render_dynamic_animations(RenderContext* renderCtx)
		{

		}

		namespace scene_renderer
		{
			globals::GlobalUniquePtr<SceneRenderer2D> g_scene_renderer;
			void init(globals::GlobalUniquePtr<SceneRenderer2D> renderer)
			{
				g_scene_renderer = rsl::move(renderer);
			}
			SceneRenderer2D* instance()
			{
				return g_scene_renderer.get();
			}
			void shutdown()
			{
				g_scene_renderer.reset();
			}
		}
	}
}
