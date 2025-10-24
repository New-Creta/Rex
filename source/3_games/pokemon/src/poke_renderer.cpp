#include "pokemon/poke_renderer.h"

#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "pokemon/render_constants.h"

namespace pokemon
{
	PokemonRenderer::PokemonRenderer(const rex::TilesetAsset* tileset)
	{
		// draw pass to render the static tiles that don't change
		// this is essentially just the background
		rex::gfx::BlockRenderPassDynamicInputs block_pass_inputs{};
		block_pass_inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
		block_pass_inputs.tileset = tileset;
		block_pass_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };

		m_block_render_pass = rex::gfx::renderer::instance()->add_render_pass<rex::gfx::BlockRenderPass>(block_pass_inputs);

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

	void PokemonRenderer::update_params(const PokemonRendererParams& params)
	{
		m_block_render_pass->update_tilemap(params.blockpass_params);
	}

}