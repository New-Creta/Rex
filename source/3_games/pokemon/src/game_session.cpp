#include "pokemon/game_session.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/text_processing/json.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_engine/shapes/rect.h"

#include "pokemon/poke_structs.h"
#include "pokemon/render_constants.h"
#include "pokemon/poke_renderer.h"

#include "rex_std/bonus/math.h"
#include "rex_std/bonus/math/point.h"

#include "rex_std/unordered_map.h"

#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"

#include "rex_engine/gfx/rendering/scene_renderer_2d.h"

#include "rex_engine/profiling/timer.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/math/coords.h"
#include "rex_engine/engine/asset_db.h"

#include "rex_engine/assets/flipbook.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/input/key_down.h"

#include "rex_engine/gfx/rendering/render_passes/block_pass.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogGameSession);

	GameSession::GameSession()
	{
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed
		SaveFile startup_save_file = load_startup_savefile();

		init_map(startup_save_file);
		init_player(startup_save_file);

		// In the future this render graph should be provided by the engine
		// the game provides the engine information what kind of render graph is needed for the game
		// the engine will then pick one of the prebuild render graphs
		init_renderer();
	}

	void GameSession::update()
	{
		m_player_character->tick(0.1f);
		//clamp_player_pos();

		draw();
	}

	SaveFile GameSession::load_startup_savefile() const
	{
		// The startup save file is located at the root directory of the project
		rsl::string startup_save_filepath(rex::cmdline::instance()->get_argument("StartupSaveFile").value_or(rex::path::join(rex::engine::instance()->project_root(), "startup_save_file.json")));
		REX_ASSERT_X(rex::file::exists(startup_save_filepath), "startup save filepath does not exist.");
		return SaveFile(startup_save_filepath);
	}

	void GameSession::init_map(const SaveFile& saveFile)
	{
		m_active_map = rex::asset_db::instance()->load<rex::Map>(saveFile.current_map_filepath);
		m_scene_blockmap = rsl::make_unique<GameBlockMap>(m_active_map);
		on_map_change();
	}

	void GameSession::init_player(const SaveFile& saveFile)
	{
		rex::WorldCoordConverter world_coord_converter = m_active_map->create_world_coord_converter();
		
		m_player_character = rsl::make_unique<PlayerCharacter>();
		m_player_character->set_pos(world_coord_converter.to_pixel_coord(saveFile.position));
	}

	void GameSession::init_renderer()
	{
		// draw pass to render the static tiles that don't change
		// this is essentially just the background
		//rex::gfx::BlockRenderPassDynamicInputs block_pass_inputs{};
		//block_pass_inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
		//block_pass_inputs.tileset = m_active_map->blockset()->tileset();
		//block_pass_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles)};

		//m_block_render_pass = rex::gfx::renderer::instance()->add_render_pass<rex::gfx::BlockRenderPass>(block_pass_inputs);

		// draw the animated tiles
		// the player, NPCs, flowers, ...
		// sprite movement in old games is found in movement.asm
		//rex::gfx::AnimatedSpritesPassDynamicInputs animated_sprites_inputs{};
		//animated_sprites_inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
		//animated_sprites_inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles) };
		//m_animated_sprites_pass = rex::gfx::renderer::instance()->add_render_pass<rex::gfx::AnimatedSpritesPass>(animated_sprites_inputs);

		// draw the water
		// water is animated by bit shifting its pixels
		//m_water_pass = rex::gfx::renderer::instance()->add_pass(rsl::make_unique<rex::gfx::BlockRenderPass>(inputs));

		// draw the UI
		//m_ui_pass = rex::gfx::renderer::instance()->add_render_pass(rsl::make_unique<UiPass>());
	}

	void GameSession::draw()
	{
		rex::PixelCoord top_left = m_player_character->pos();
		top_left.x -= 64; // 8 tiles
		top_left.y -= 64; // 8 tiles
		//rsl::point<rex::TileCount> top_left{};
		//top_left.x = rex::TileCount(m_player_character->pos().x);
		//top_left.y = rex::TileCount(m_player_character->pos().y);

		rex::gfx::SceneRenderParams params{};
		params.tiles_source = m_scene_blockmap->tiles();
		params.top_left = top_left;
		params.coord_converter = m_active_map->create_world_coord_converter();
		params.world_width_in_tiles.get() = m_scene_blockmap->width().get();

		rex::gfx::scene_renderer::instance()->update_params(params);

		//PokemonRendererParams params{};
		//params.blockpass_params.tiles_source = m_scene_blockmap->tiles();
		//params.blockpass_params.top_left_start = top_left;
		//params.blockpass_params.world_width_in_tiles = m_scene_blockmap->width().get();

		//m_renderer->update_params(params);

		//m_block_render_pass->update_tilemap(params);
		//auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		//m_block_render_pass->render(render_ctx.get());
	}

	// temporary function, this should be handled with collision detection in the future
	void GameSession::clamp_player_pos()
	{
		rex::WorldCoordConverter map_coord_converter = m_active_map->create_world_coord_converter();

		rex::PixelCoord player_pos = m_player_character->pos();
		rex::TileCoord player_pos_in_tiles = map_coord_converter.to_tile_coord(player_pos);

		rsl::pointi8 min_player_pos{}; //constants::player_render_position_top_left;
		rsl::pointi8 max_player_pos{}; //constants::player_render_position_bottom_right;
		rsl::point<rex::TileCount> size = rex::size_in_tiles(m_active_map);

		if (player_pos_in_tiles.x < 0 || player_pos_in_tiles.x > size.x.get() ||
			player_pos_in_tiles.y < 0 || player_pos_in_tiles.y > size.y.get())
		{
			player_pos_in_tiles.x = rsl::clamp_min(player_pos_in_tiles.x, player_pos_in_tiles.x);
			player_pos_in_tiles.y = rsl::clamp_min(player_pos_in_tiles.y, player_pos_in_tiles.y);


			player_pos_in_tiles.x = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos_in_tiles.x), size.x.get() - player_pos_in_tiles.x));
			player_pos_in_tiles.y = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos_in_tiles.y), size.y.get() - player_pos_in_tiles.y));

			m_player_character->set_pos(map_coord_converter.to_pixel_coord(player_pos_in_tiles));
		}

	}

	void GameSession::on_map_change()
	{
		rex::gfx::scene_renderer::instance()->notify_new_tileset(m_active_map->blockset()->tileset());

		rsl::point<f32> zoom{};

		zoom.x = rex::gfx::gal::instance()->back_buffer_width() / constants::g_screen_width;
		zoom.y = rex::gfx::gal::instance()->back_buffer_height() / constants::g_screen_height;

		rex::gfx::scene_renderer::instance()->update_zoom(zoom);
	}
}