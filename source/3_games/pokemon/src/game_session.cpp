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

#include "rex_engine/gfx/rendering/render_passes/background_pass.h"

#include "pokemon/oam.h"

#include "pokemon/tiles.h"
#include "pokemon/tile_renderer.h"

#include "pokemon/game_state/battle_state.h"
#include "pokemon/game_state/blackborders_state.h"
#include "pokemon/game_state/copyright_state.h"
#include "pokemon/game_state/fight_scene_state.h"
#include "pokemon/game_state/gamefreak_logo_state.h"
#include "pokemon/game_state/oak_intro_state.h"
#include "pokemon/game_state/overworld_state.h"
#include "pokemon/game_state/start_menu_state.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogGameSession);

	
	GameSession::GameSession()
		: m_state_info()
	{
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed

		init_globals();
		init_state_tree();
	}
	GameSession::~GameSession()
	{
		tiles::shutdown();
	}

	void GameSession::update()
	{
		GameLoopState next_state = m_state_tree[m_state_info.state].tick(m_state_info.num_frames_active);

		if (next_state != m_state_info.state)
		{
			m_state_tree[m_state_info.state].end();
			m_state_info.state = next_state;
			m_state_tree[m_state_info.state].begin();
			m_state_info.num_frames_active = 0;
		}
		else
		{
			m_state_info.num_frames_active++;
		}
	}

	void GameSession::init_globals()
	{
		tiles::init(rex::globals::make_unique<TilesManager>());
		tile_renderer::init(rex::globals::make_unique<TileRenderer>());
	}
	void GameSession::init_state_tree()
	{
		// Original Pokemon game loop:
		// 
		// Intro 
		// -> Copyright
		// -> Gamefreak logo
		//		-> Shooting start from top right to bottom left
		//		-> Flash of gamefreak logo
		//		-> Load small stars
		// -> Gangar + Nidorino fight
		// 
		// Title Screen
		// -> loops through pokemon, awaiting user input
		// 
		// Main Menu
		// -> detects if an existing save is present
		// -> if so add "continue" to main menu and loads main menu
		// -> if not, load Prof oak intro and goes through that
		// 
		// Overworld loop
		// -> To much to go into detail, but this the main game loop
		//
		// Battle loop
		// -> Is loaded on top of overworld loop
		// -> Keeps looping until battle has finished

		m_state_info.state = GameLoopState::Copyright;

		m_state_tree.emplace(GameLoopState::Copyright, CopyrightState());
		m_state_tree.emplace(GameLoopState::BlackBorders, BlackBordersState());
		m_state_tree.emplace(GameLoopState::GamefreakLogo, GamefreakLogoState());
		m_state_tree.emplace(GameLoopState::Fight, FightSceneState());
		m_state_tree.emplace(GameLoopState::StartMenu, StartMenuState());
		m_state_tree.emplace(GameLoopState::OakIntro, OakIntroState());
		m_state_tree.emplace(GameLoopState::Overworld, OverworldState());
		m_state_tree.emplace(GameLoopState::Battle, BattleState());
	}

	SaveFile GameSession::load_startup_savefile() const
	{
		rex::scratch_string default_startup_save_filepath = rex::path::join(rex::engine::instance()->project_root(), "startup_save_file.json");
		rsl::string startup_save_filepath(rex::cmdline::instance()->get_argument("StartupSaveFile").value_or(default_startup_save_filepath));
		REX_ASSERT_X(rex::file::exists(startup_save_filepath), "startup save filepath does not exist.");
		return SaveFile(startup_save_filepath);
	}
}