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
		m_player_character2 = rsl::make_unique<PlayerCharacter>();
		m_player_character2->set_pos(m_active_map->create_world_coord_converter().to_pixel_coord(rex::TileCoord({ 24, 24 })));
		m_player_character3 = rsl::make_unique<PlayerCharacter>();
		m_player_character3->set_pos(m_active_map->create_world_coord_converter().to_pixel_coord(rex::TileCoord({ 26, 24 })));

		init_camera();
	}

	void GameSession::update()
	{
		f32 dt = rex::engine::instance()->frame_info().delta_time().to_milliseconds();

		// Tick all objects in the scene that require ticking
		m_player_character->tick(dt);
		m_camera->set_pos(m_player_character->pos());

		//clamp_player_pos();

		// this likely doesn't need to happen every frame..
		update_render_info();
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

		zoom.x = rex::gfx::gal::instance()->back_buffer_width() / constants::g_screen_width;
		zoom.y = rex::gfx::gal::instance()->back_buffer_height() / constants::g_screen_height;

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

	// temporary function, this should be handled with collision detection in the future
	void GameSession::clamp_player_pos()
	{
		rex::WorldCoordConverter map_coord_converter = m_active_map->create_world_coord_converter();

		rex::PixelCoord player_pos = m_player_character->pos();
		rex::TileCoord player_pos_in_tiles = map_coord_converter.to_tile_coord(player_pos);

		rsl::pointi8 min_player_pos{};
		rsl::pointi8 max_player_pos{};
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

}