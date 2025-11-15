#include "rex_engine/gfx/rendering/scene_renderer_2d.h"

#include "rex_engine/gfx/system/resource_manager.h"

namespace rex
{
	namespace gfx
	{
		SceneRenderer2D::SceneRenderer2D()
		{
			m_render_target = rex::gfx::gal::instance()->create_render_target(
				gal::instance()->back_buffer_width(),
				gal::instance()->back_buffer_height(),
				TextureFormat::Unorm4Srgb
				);

			// draw pass to render the static tiles that don't change
			// this is essentially just the background
			BlockRenderPassCreationInfo blockpass_creation_info{};
			blockpass_creation_info.render_target = m_render_target.get();

			m_block_render_pass = rsl::make_unique<BlockRenderPass>(blockpass_creation_info);

			// draw the animated tiles
			// the player, NPCs, flowers, ...
			// sprite movement in old games is found in movement.asm
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
			auto render_ctx = gal::instance()->new_render_ctx(nullptr, "2D Scene Render");

			// This is likely not needed
			BackBufferRenderTarget* render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
			render_ctx->set_render_target(render_target);
			render_ctx->clear_render_target(render_target);

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

			m_block_render_pass->update_params(params);
			m_animated_sprites_pass->update_params(params);
		}

		const SceneRenderParams& SceneRenderer2D::current_params() const
		{
			return m_params;
		}

		void SceneRenderer2D::render_tilemap(RenderContext* renderCtx)
		{
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
