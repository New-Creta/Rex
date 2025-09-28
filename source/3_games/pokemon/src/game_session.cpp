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

#include "pokemon/assets/database.h"

#include "rex_engine/shapes/rect.h"

#include "pokemon/poke_structs.h"
#include "pokemon/map_matrix.h"
#include "pokemon/render_constants.h"

#include "rex_std/bonus/math.h"
#include "rex_std/bonus/math/point.h"

#include "rex_std/unordered_map.h"


#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"

#include "pokemon/block.h"
#include "pokemon/blockset.h"
#include "pokemon/tileset.h"
#include "pokemon/poke_map.h"

#include "rex_engine/profiling/timer.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/engine/asset_db.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/input/key_down.h"

#include "rex_engine/gfx/rendering/render_passes/tile_pass.h"
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
		init_render_pass();
	}

	void GameSession::update()
	{
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
		m_blockmap = rsl::make_unique<GameBlockMap>(m_active_map);
	}

	void GameSession::init_player(const SaveFile& saveFile)
	{
		m_player_position = saveFile.position;

	}

	void GameSession::init_input()
	{
		rex::event_system::instance()->subscribe<rex::KeyDown>(
			[this](const rex::KeyDown& ev)
			{
				switch (ev.key())
				{
				case rex::KeyCode::W:
					m_player_position.y -= 1;
					break;
				case rex::KeyCode::S:
					m_player_position.y += 1;
					break;
				case rex::KeyCode::A:
					m_player_position.x -= 1;
					break;
				case rex::KeyCode::D:
					m_player_position.x += 1;
					break;
				default:
					break;
				}

				rsl::pointi8 min_player_pos = constants::player_render_position_top_left;
				rsl::pointi8 max_player_pos = constants::player_render_position_bottom_right;

				m_player_position.x = rsl::clamp_min(m_player_position.x, min_player_pos.x);
				m_player_position.y = rsl::clamp_min(m_player_position.y, min_player_pos.y);

				m_player_position.x = static_cast<s8>(rsl::clamp_max(static_cast<s32>(m_player_position.x), m_active_map->width_in_tiles() - max_player_pos.x));
				m_player_position.y = static_cast<s8>(rsl::clamp_max(static_cast<s32>(m_player_position.y), m_active_map->height_in_tiles() - max_player_pos.y));

			});
	}

	void GameSession::init_render_pass()
	{
		// the block/tile pass needs the following information
		// - how many tiles will we render on screen
		// - where will we render to
		// - what tileset will we use for rendering
		// All other information can be infered from the above
		// if we render 20x18 tiles, 1 tile's width is 1/20th of the render target's width
		// and it's 1/18th of the render target's height

		rex::gfx::BlockRenderPassDynamicInputs inputs{};
		inputs.render_target = rex::gfx::gal::instance()->backbuffer_rendertarget();
		inputs.tileset = m_active_map->blockset()->tileset();
		inputs.screen_resolution = { rex::TileCount(constants::g_screen_width_in_tiles), rex::TileCount(constants::g_screen_height_in_tiles)};

		m_block_render_pass = rsl::make_unique<rex::gfx::BlockRenderPass>(inputs);
	}

	void GameSession::draw()
	{
		rsl::point<rex::TileCount> top_left{};
		top_left.x = rex::TileCount(m_player_position.x);
		top_left.y = rex::TileCount(m_player_position.y);

		rex::gfx::BlockRenderPassUpdateParams params{};
		//params.screen_resolution = { 64, 128/* 724 / constants::g_tile_width_px / 2, 724 / constants::g_tile_height_px / 2*/ };
		params.tiles_source = m_blockmap->tiles();
		params.top_left_start = top_left;
		params.world_width_in_tiles = m_blockmap->width().get();
		m_block_render_pass->update_tilemap(params);

		auto render_ctx = rex::gfx::gal::instance()->new_render_ctx();
		m_block_render_pass->render(render_ctx.get());
	}
}