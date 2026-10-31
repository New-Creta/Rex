#include "pokemon/pkmn_renderer.h"

#include "rex_engine/gfx/graphics.h"

namespace pokemon
{
	void Renderer::render()
	{
		auto ctx = rex::gfx::gal::instance()->new_render_ctx();

		tiles::instance()->pre_pass();
		tiles::instance()->run_pass(ctx.get());
		tiles::instance()->post_pass();
	}
}