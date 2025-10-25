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
			
		}

		Renderer::~Renderer() = default;

		void Renderer::render()
		{
			//auto render_ctx = gal::instance()->new_render_ctx();

			//for (rsl::unique_ptr<RenderPass>& pass : m_passes)
			//{
			//	pass->pre_pass();
			//	pass->run_pass(render_ctx.get());
			//	pass->post_pass();
			//}
		}

		namespace renderer
		{
			//globals::GlobalUniquePtr<Renderer> g_renderer;
			//void init(globals::GlobalUniquePtr<Renderer> renderer)
			//{
			//	g_renderer = rsl::move(renderer);
			//}
			//Renderer* instance()
			//{
			//	return g_renderer.get();
			//}
			//void shutdown()
			//{
			//	g_renderer.reset();
			//}
		}
	}
}