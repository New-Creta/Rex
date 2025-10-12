#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "rex_engine/filesystem/path.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	namespace gfx
	{
		Renderer::Renderer()
		{
			AnimatedSpritesPassDynamicInputs inputs{};
			inputs.render_target = gal::instance()->backbuffer_rendertarget();
			inputs.screen_resolution.x.get() = 20;
			inputs.screen_resolution.y.get() = 18;
			m_animated_sprites_pass = rsl::make_unique<AnimatedSpritesPass>(inputs);
		}

		Renderer::~Renderer() = default;

		AnimatedSprite* Renderer::add_animated_sprite(rsl::unique_ptr<AnimatedSprite> sprite)
		{
			return m_animated_sprites_pass->add_sprite(rsl::move(sprite));
		}

		namespace renderer
		{
			globals::GlobalUniquePtr<Renderer> g_renderer;
			void init(globals::GlobalUniquePtr<Renderer> renderer)
			{
				g_renderer = rsl::move(renderer);
			}
			Renderer* instance()
			{
				return g_renderer.get();
			}
			void shutdown()
			{
				g_renderer.reset();
			}
		}
	}
}