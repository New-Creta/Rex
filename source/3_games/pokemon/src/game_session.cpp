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
		GameLoopState new_state = m_state_tree[m_state_info.state].tick();

		if (new_state != m_state_info.state)
		{
			m_state_tree[m_state_info.state].end();
			m_state_info.state = new_state;
			m_state_tree[m_state_info.state].begin();
			m_state_info.num_frames_active = 0;
		}
		else
		{
			m_state_info.num_frames_active++;
		}



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

		rsl::unique_ptr<GameLoop> new_game_loop = m_game_loop->run();
		if (new_game_loop)
		{
			m_game_loop->end();
			m_game_loop = rsl::move(new_game_loop);
			m_game_loop->begin();
		}



















		// Tick all objects in the scene that require ticking
		m_player_character->tick();
		m_camera->set_pos(m_player_character->pos());

		// this likely doesn't need to happen every frame..
		update_render_info();



		// The original game doesn't go through an update loop when a script is active.
		// The game will change a sprite position and state and this is immediately reflected through tile rendering
		// Meaning if a script is active to move a sprite from A to B, that'll be all the game is doing at the time
		// it will not update any NPCs, not process input, not animate water, ..






		// Objective: Add NPCs
		// Objective: Add collision

		
		
		// Game tick is as followed
		// 
		// 1. If the player is still animating, tick the animation and advance to the next frame
		//
		// 2. Check if we're in the safari zone, if so check if the timer has expired
		//
		// 3. Check for status flags
		//
		// 4. Check if we have an opponent
		// 
		// 5. Check more status flags
		//
		// 6. Check if start is pressed


		
	}

	void GameSession::init_globals()
	{
		tiles::init(rex::globals::make_unique<TilesManager>());
		tile_renderer::init(rex::globals::make_unique<TileRenderer>());
	}
	void GameSession::init_state_tree()
	{
		m_state_info.state = GameLoopState::Copyright;

		m_state_tree.emplace(GameLoopState::Copyright, StateTask(
			[this]() { begin_copyright(); },
			[this]() { return tick_copyright(); },
			[this]() { end_copyright(); }
		));
		m_state_tree.emplace(GameLoopState::BlackBorders, StateTask(
			[this]() { begin_black_borders(); },
			[this]() { return tick_black_borders(); },
			[this]() { end_black_borders(); }
		));
		m_state_tree.emplace(GameLoopState::GamefreakLogo, StateTask(
			[this]() { begin_gamefreak(); },
			[this]() { return tick_gamefreak(); },
			[this]() { end_gamefreak(); }
		));
		m_state_tree.emplace(GameLoopState::Fight, StateTask(
			[this]() { begin_fight(); },
			[this]() { return tick_fight(); },
			[this]() { end_fight(); }
		));
		m_state_tree.emplace(GameLoopState::StartMenu, StateTask(
			[this]() { begin_startmenu(); },
			[this]() { return tick_startmenu(); },
			[this]() { end_startmenu(); }
		));
		m_state_tree.emplace(GameLoopState::OakIntro, StateTask(
			[this]() { begin_oak_intro(); },
			[this]() { return tick_oak_intro(); },
			[this]() { end_oak_intro(); }
		));
		m_state_tree.emplace(GameLoopState::Overworld, StateTask(
			[this]() { begin_overworld(); },
			[this]() { return tick_overworld(); },
			[this]() { end_overworld(); }
		));
		m_state_tree.emplace(GameLoopState::Battle, StateTask(
			[this]() { begin_battle(); },
			[this]() { return tick_battle(); },
			[this]() { end_battle(); }
		));
	}

	SaveFile GameSession::load_startup_savefile() const
	{
		rex::scratch_string default_startup_save_filepath = rex::path::join(rex::engine::instance()->project_root(), "startup_save_file.json");
		rsl::string startup_save_filepath(rex::cmdline::instance()->get_argument("StartupSaveFile").value_or(default_startup_save_filepath));
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
		rex::WorldCoordConverter world_coord_converter = m_active_map->create_world_coord_converter();
		
		m_player_character = rsl::make_unique<PlayerCharacter>();
		m_player_character->set_pos(world_coord_converter.to_pixel_coord(saveFile.position));
	}
	void GameSession::init_camera()
	{
		rex::PixelCoord res_size{};
		res_size.x = constants::g_screen_width;
		res_size.y = constants::g_screen_height;

		rex::PixelCoord look_ahead{};
		look_ahead.x = constants::g_camera_look_ahead.x;
		look_ahead.y = constants::g_camera_look_ahead.y;

		glm::vec2 zoom{};

		zoom.x = static_cast<f32>(rex::gfx::gal::instance()->back_buffer_width() / constants::g_screen_width);
		zoom.y = static_cast<f32>(rex::gfx::gal::instance()->back_buffer_height() / constants::g_screen_height);

		m_camera = rsl::make_unique<rex::gfx::Camera2D>(res_size, look_ahead, zoom);
	}

	void GameSession::update_render_info()
	{
		rex::gfx::SceneRenderParams params{};
		params.tileset = m_active_map->blockset()->tileset();
		params.tiles_source = m_scene_blockmap->tiles();
		params.camera = m_camera.get();
		params.coord_converter = m_active_map->create_world_coord_converter();
		params.world_width_in_tiles.get() = m_scene_blockmap->width().get();

		rex::gfx::scene_renderer::instance()->update_params(params);
	}

	void GameSession::begin_copyright()
	{
		// Load the copyright tiles into memory and send it to the gpu
		rex::scratch_string copyright_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "copyright.png");
		rex::scratch_string gamefreak_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "title", "gamefreak_inc.png");
		
		s32 dst_start = 0;
		dst_start += tiles::instance()->load(copyright_tiles, dst_start);
		dst_start += tiles::instance()->load(gamefreak_tiles, dst_start);
		tiles::instance()->copy_to_gpu();

		tile_renderer::instance()->set_indices(
			2, 7,
			{
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,										// ©'95.'96.'98 Nintendo
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72,				// ©'95.'96.'98 	 inc.
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, // ©'95.'96.'98 GAME FREAK inc.
			}
		);
	}
	pokemon::GameLoopState GameSession::tick_copyright()
	{
		const s16 num_frames_to_be_active = 180;
		if (m_state_info.num_frames_active > num_frames_to_be_active)
		{
			return pokemon::GameLoopState::BlackBorders;
		}
		return pokemon::GameLoopState::Copyright;
	}
	void GameSession::end_copyright()
	{

	}

	void GameSession::begin_black_borders()
	{
		// gengar tilemap holds the black tile which is required for the black border rendering
		rex::scratch_string gengar_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "gengar.png");

		rex::scratch_string nidorino_tiles_1 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_1.png");
		rex::scratch_string nidorino_tiles_2 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_2.png");
		rex::scratch_string nidorino_tiles_3 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_3.png");

		s16 dst_start = 0;
		dst_start += tiles::instance()->load(gengar_tiles, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_1, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_2, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_3, dst_start);
		tiles::instance()->copy_to_gpu();

	}
	GameLoopState GameSession::tick_black_borders()
	{
		const s16 num_frames_to_be_active = 64;
		if (m_state_info.num_frames_active > num_frames_to_be_active)
		{
			return pokemon::GameLoopState::GamefreakLogo;
		}
		return pokemon::GameLoopState::BlackBorders;
	}
	void GameSession::end_black_borders()
	{

	}

	void GameSession::begin_gamefreak()
	{
		rex::scratch_string gamefreak_text = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "gamefreak_presents.png");
		rex::scratch_string gamefreak_logo = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "gamefreak_logo.png");
		rex::scratch_string shooting_star = rex::path::join(rex::engine::instance()->project_root(), "gfx", "battle", "move_anim_1.png");
		rex::scratch_string falling_star = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "falling_star.png");

		tiles::load(gamefreak_text);
		tiles::load(gamefreak_logo);
		tiles::load(shooting_star, 0x03); // copy the third tile in the list, which is the top left quadrant of the star
		tiles::load(shooting_star, 0x13); // copy the 13th tile in the list, which is the bottom left quadarant of the star
		tiles::load(falling_star);

		// gamefreak logo OAM data
		oam::add(OAMStruct{10,  9, 0, 0, 0x8d, 0});
		oam::add(OAMStruct{11,  9, 0, 0, 0x8e, 0});
		oam::add(OAMStruct{10, 10, 0, 0, 0x8f, 0});
		oam::add(OAMStruct{11, 10, 0, 0, 0x90, 0});
		oam::add(OAMStruct{10, 11, 0, 0, 0x91, 0});
		oam::add(OAMStruct{11, 11, 0, 0, 0x92, 0});
		oam::add(OAMStruct{ 6, 12, 0, 0, 0x80, 0});
		oam::add(OAMStruct{ 7, 12, 0, 0, 0x81, 0});
		oam::add(OAMStruct{ 8, 12, 0, 0, 0x82, 0});
		oam::add(OAMStruct{ 9, 12, 0, 0, 0x83, 0});
		oam::add(OAMStruct{10, 12, 0, 0, 0x93, 0});
		oam::add(OAMStruct{11, 12, 0, 0, 0x84, 0});
		oam::add(OAMStruct{12, 12, 0, 0, 0x85, 0});
		oam::add(OAMStruct{13, 12, 0, 0, 0x83, 0});
		oam::add(OAMStruct{14, 12, 0, 0, 0x81, 0});
		oam::add(OAMStruct{15, 12, 0, 0, 0x86, 0});

		// shooting start OAM data
		oam::add(OAMStruct{20, 0, 0, 0, 0xa0, OAMAttributes::DmgPalette });
		oam::add(OAMStruct{20, 0, 0, 0, 0xa0, OAMAttributes::DmgPalette | OAMAttributes::FlipX });
		oam::add(OAMStruct{20, 0, 0, 0, 0xa1, OAMAttributes::DmgPalette });
		oam::add(OAMStruct{20, 0, 0, 0, 0xa1, OAMAttributes::DmgPalette | OAMAttributes::FlipX });

		oam::add(OAMStruct{ 0, 0, 0, 0, 0xa2, OAMAttributes::BehindBg | OAMAttributes::DmgPalette });

	}
	pokemon::GameLoopState GameSession::tick_gamefreak()
	{

	}
	void GameSession::end_gamefreak()
	{

	}

	void GameSession::begin_fight()
	{

	}
	pokemon::GameLoopState GameSession::tick_fight()
	{

	}
	void GameSession::end_fight()
	{

	}

	void GameSession::begin_startmenu()
	{

	}
	pokemon::GameLoopState GameSession::tick_startmenu()
	{

	}
	void GameSession::end_startmenu()
	{

	}

	void GameSession::begin_oak_intro()
	{

	}
	pokemon::GameLoopState GameSession::tick_oak_intro()
	{

	}
	void GameSession::end_oak_intro()
	{

	}

	void GameSession::begin_overworld()
	{

	}
	pokemon::GameLoopState GameSession::tick_overworld()
	{

	}
	void GameSession::end_overworld()
	{

	}

	void GameSession::begin_battle()
	{

	}
	pokemon::GameLoopState GameSession::tick_battle()
	{

	}
	void GameSession::end_battle()
	{

	}
}