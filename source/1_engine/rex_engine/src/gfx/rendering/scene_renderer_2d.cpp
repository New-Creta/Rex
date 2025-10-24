#include "rex_engine/gfx/rendering/scene_renderer_2d.h"

#include "rex_engine/gfx/rendering/"

namespace rex
{
	namespace gfx
	{
		SceneRenderer2D::SceneRenderer2D()
		{
			// draw pass to render the static tiles that don't change
			// this is essentially just the background
			rex::gfx::BlockRenderPassDynamicInputs block_pass_inputs{};
			block_pass_inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
			//block_pass_inputs.tileset = tileset;
			block_pass_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };

			m_block_render_pass = rsl::make_unique<BlockRenderPass>(block_pass_inputs);

			// draw the animated tiles
			// the player, NPCs, flowers, ...
			// sprite movement in old games is found in movement.asm
			rex::gfx::AnimatedSpritesPassDynamicInputs animated_sprites_inputs{};
			animated_sprites_inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
			animated_sprites_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };
			m_animted_sprites_pass = rex::gfx::renderer::instance()->add_render_pass<rex::gfx::AnimatedSpritesPass>(animated_sprites_inputs);

			// draw the water
			// water is animated by bit shifting its pixels
			//m_water_pass = rex::gfx::renderer::instance()->add_pass(rsl::make_unique<rex::gfx::BlockRenderPass>(inputs));

			// draw the UI
			//m_ui_pass = rex::gfx::renderer::instance()->add_render_pass(rsl::make_unique<UiPass>());

		}

		void SceneRenderer2D::render()
		{
			render_tilemap();
			render_flipbook_animations();
			render_dynamic_animations();
		}

		void SceneRenderer2D::add_animated_sprite(rsl::unique_ptr<AnimatedSprite> animatedSprite)
		{
			m_animated_sprites.emplace_back(rsl::move(animatedSprite));
		}

		void SceneRenderer2D::notify_new_tileset(const TilesetAsset* tileset)
		{
			BlockRenderPassDynamicInputs inputs{};
			inputs.tileset = tileset;

			m_block_render_pass->update_dynamic_inputs(inputs);
		}

		void SceneRenderer2D::render_tilemap()
		{
			BlockRenderPassUpdateParams params{};
			params.tiles_source = m_scene_tilemap->tiles();
			params.top_left_start = m_top_left;
			params.world_width_in_tiles = m_scene_tilemap->width().get();

			m_block_render_pass->update_tilemap(params);
			auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
			m_block_render_pass->render(render_ctx.get());
		}

		void SceneRenderer2D::render_flipbook_animations()
		{

		}

		void SceneRenderer2D::render_dynamic_animations()
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
