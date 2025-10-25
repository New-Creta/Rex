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
		init_input();

		// In the future this render graph should be provided by the engine
		// the game provides the engine information what kind of render graph is needed for the game
		// the engine will then pick one of the prebuild render graphs
		init_renderer();
	}

	void GameSession::update()
	{
		f32 dt = 0.1f;
		m_player_character->tick(dt);
		clamp_player_pos();

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
	}

	void GameSession::init_player(const SaveFile& saveFile)
	{
		m_player_character = rsl::make_unique<PlayerCharacter>();
		m_player_character->set_pos(saveFile.position);
	}

	void GameSession::init_input()
	{
		rex::event_system::instance()->subscribe<rex::KeyDown>(
			[this](const rex::KeyDown& ev)
			{
				rex::InputInfo input_info{};
				input_info.action.type = rex::InputActionType::Key;
				input_info.action.data.key_code = ev.key();
				input_info.ticks_pressed = 0;

				m_player_character->handle_input(input_info);

				// we should increase the input 2 pixels per key stroke
				// this is because in the old pokemon games a single animation takes 4 ticks
				// given that a tile is 8x8 pixels, to get through a single tile, 
				// the game has to translate the camera 2 pixels per tick
				// the player does have to play 2 tiles at a time (which is the width of a single square)

				//switch (ev.key())
				//{
				//case rex::KeyCode::W:
				//	m_player_position.y -= 1;
				//	break;
				//case rex::KeyCode::S:
				//	m_player_position.y += 1;
				//	break;
				//case rex::KeyCode::A:
				//	m_player_position.x -= 1;
				//	break;
				//case rex::KeyCode::D:
				//	m_player_position.x += 1;
				//	break;
				//default:
				//	break;
				//}

				//TileCoord player_pos = m_player_character->pos();

				//rsl::pointi8 min_player_pos{}; //constants::player_render_position_top_left;
				//rsl::pointi8 max_player_pos{}; //constants::player_render_position_bottom_right;

				//player_pos.x = rsl::clamp_min(player_pos.x, min_player_pos.x);
				//player_pos.y = rsl::clamp_min(player_pos.y, min_player_pos.y);

				//rsl::point<rex::TileCount> size = rex::size_in_tiles(m_active_map);

				//player_pos.x = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos.x), size.x.get() - max_player_pos.x));
				//player_pos.y = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos.y), size.y.get() - max_player_pos.y));

				//m_player_character->set_pos(player_pos);
			});
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
		//m_animted_sprites_pass = rex::gfx::renderer::instance()->add_render_pass<rex::gfx::AnimatedSpritesPass>(animated_sprites_inputs);

		// draw the water
		// water is animated by bit shifting its pixels
		//m_water_pass = rex::gfx::renderer::instance()->add_pass(rsl::make_unique<rex::gfx::BlockRenderPass>(inputs));

		// draw the UI
		//m_ui_pass = rex::gfx::renderer::instance()->add_render_pass(rsl::make_unique<UiPass>());
	}

	void GameSession::draw()
	{
		rsl::point<rex::TileCount> top_left{};
		top_left.x = rex::TileCount(m_player_character->pos().x);
		top_left.y = rex::TileCount(m_player_character->pos().y);

		rex::gfx::SceneRenderParams params{};
		params.tiles_source = m_scene_blockmap->tiles();
		params.top_left = top_left;
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
		TileCoord player_pos = m_player_character->pos();

		rsl::pointi8 min_player_pos{}; //constants::player_render_position_top_left;
		rsl::pointi8 max_player_pos{}; //constants::player_render_position_bottom_right;

		player_pos.x = rsl::clamp_min(player_pos.x, min_player_pos.x);
		player_pos.y = rsl::clamp_min(player_pos.y, min_player_pos.y);

		rsl::point<rex::TileCount> size = rex::size_in_tiles(m_active_map);

		player_pos.x = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos.x), size.x.get() - max_player_pos.x));
		player_pos.y = static_cast<s8>(rsl::clamp_max(static_cast<s32>(player_pos.y), size.y.get() - max_player_pos.y));

		m_player_character->set_pos(player_pos);
	}

	void GameSession::on_map_change()
	{
		rex::gfx::scene_renderer::instance()->notify_new_tileset(m_active_map->blockset()->tileset());


	}
}