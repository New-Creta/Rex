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
		init_camera();
	}

	void GameSession::update()
	{
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
}