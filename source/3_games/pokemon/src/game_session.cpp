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

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogGameSession);

	rsl::unique_ptr<rex::gfx::Texture2D> create_tileset_texture(rsl::string_view filepath)
	{
		return nullptr;
		//rex::ImageLoadResult tileset_img_load_res = rex::load_image(filepath);

		//// A tileset only holds 1 channel, we have to convert it to 4 channels as that's what the GPU expects
		//rsl::unique_array<rsl::Rgba> tileset_rgba = rsl::make_unique<rsl::Rgba[]>(tileset_img_load_res.width * tileset_img_load_res.height * sizeof(rsl::Rgba));
		//for (s32 color_idx = 0; color_idx < tileset_img_load_res.width * tileset_img_load_res.height; ++color_idx)
		//{
		//	u8 color = tileset_img_load_res.data[color_idx];
		//	rsl::Rgba& rgba = tileset_rgba[color_idx];
		//	rgba.red = color;
		//	rgba.green = color;
		//	rgba.blue = color;
		//	rgba.alpha = 255;
		//}

		//rsl::unique_ptr<rex::gfx::Texture2D> tileset_texture = rex::gfx::gal::instance()->create_texture2d(tileset_img_load_res.width, tileset_img_load_res.height, rex::gfx::TextureFormat::Unorm4, tileset_rgba.get());
		//return tileset_texture;
	}

	MapRenderData load_map_render_data(rsl::string_view filepath)
	{
		rex::json::json blockset_json = rex::json::read_from_file(filepath);
		rsl::string_view tileset_filepath = blockset_json["tileset"];

		MapRenderData res{};
		res.blockset = load_blockset(blockset_json["blockset"]);
		res.tileset = create_tileset_texture(tileset_filepath);

		return res;
	}

	GameSession::GameSession()
	{
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed
		SaveFile startup_save_file = load_startup_savefile();

		m_active_map = rex::asset_db::instance()->load<rex::Map>(startup_save_file.current_map_filepath);
		m_player_position = startup_save_file.position;

		s32 total_tilemap_width = m_active_map->width_in_tiles() + (2 * constants::g_map_padding_tiles);
		s32 total_tilemap_height = m_active_map->height_in_tiles() + (2 * constants::g_map_padding_tiles);
		m_blockmap = rsl::make_unique<GameBlockMap>(m_active_map);

		//m_active_map = init_map(startup_save_file);
		//m_tile_renderer = init_tile_renderer(m_active_map->render_data());

		init_input();

		m_tile_render_pass = rsl::make_unique<rex::gfx::TileRenderPass>(rex::gfx::gal::instance()->backbuffer_rt(), m_active_map->blockset()->tileset());
	}

	void GameSession::init_tilemap()
	{

	}

	void GameSession::update()
	{
		rsl::pointi32 top_left{};
		top_left.x = m_player_position.x;
		top_left.y = m_player_position.y;

		rex::gfx::TileRenderPassParams params{};
		params.screen_resolution = { constants::g_block_width_px / constants::g_tile_width_px, constants::g_block_height_px / constants::g_tile_height_px};
		params.tiles_source = m_blockmap->tiles();
		params.top_left_start = top_left;
		params.world_width_in_tiles = m_blockmap->width_in_tiles();
		m_tile_render_pass->update_tilemap(params);

		//m_tile_renderer->update_tile_data(m_active_map->map_matrix(), m_player_position);
	}

	SaveFile GameSession::load_startup_savefile() const
	{
		// The startup save file is located at the root directory of the project
		rsl::string startup_save_filepath(rex::cmdline::instance()->get_argument("StartupSaveFile").value_or(rex::path::join(rex::engine::instance()->project_root(), "startup_save_file.json")));
		REX_ASSERT_X(rex::file::exists(startup_save_filepath), "startup save filepath does not exist.");
		return SaveFile(startup_save_filepath);
	}

	//rsl::unique_ptr<Map> GameSession::init_map(const SaveFile& startupSaveFile)
	//{
	//	// Load the map first, good to check if it exists
	//	m_active_map = rex::asset_db::instance()->load<rex::Map>(startupSaveFile.current_map_filepath);

	//	//rex::gfx::TileRenderPass render_pass(nullptr, nullptr);
	//	//rex::gfx::TileRenderPassParams params{};

	//	//params.screen_resolution = { constants::g_screen_width / 8, constants::g_screen_height / 8 };
	//	//params.tiles_source = active_map->tiles();
	//	//params.top_left_start = {}; // camera pos - player offset
	//	//params.world_width_in_tiles = active_map->width_in_tiles();

	//	//render_pass.update_tilemap(params);

	//	//MapData* map_data = asset_db::find_map(startupSaveFile.current_map_filepath);



	//	// We have a new goal here now to reduce memory usage during runtime
	//	// Create an vertex buffer holding the vertices for each tile
	//	// A vertex for a single tile consist of the following
	//	// X, Y -> 8 bytes
	//	// U, V -> 8 bytes
	//	// total: 16 bytes
	//	// The WVP matrix for each tile will be sent in a buffer of its own so we have 1 WVP for each tile instead of 1 per vertex
	//	// This will use per instance data.
	//	// The original game boy screen width is 160 x 144 pixels, meaning we're drawing 20 x 18 tiles
	//	// This results in a total of 360 tiles
	//	// Each tile has 4 vertices, each vertex is 16 bytes
	//	// 4 * 16 * 360 = 23040 bytes, this is the size of the vertex buffer
	//	// we also need to upload 1 world view projection per tile
	//	// each world view projection matrix is a 4 x 4 matrix, each matrix being 16 bytes
	//	// 360 x 16 = 5760
	//	// This means we can render the viewport using only 28 800 bytes, at least for the map itself, excluding the player sprite and NPCs

	//	// Create the map matrix, which is a matrix of the map, with the padding
	//	// The map matrix only changes when the player transitions between maps
	//	//MapMatrix map_matrix(map_data);

	//	// Load the map render data, which is the tileset and blockset into memory
	//	//MapRenderData map_render_data = load_map_render_data(map_data->map_header.map_render_data_filepath);

	//	// Load all the warp and bg objects

	//	// Load all the NPC information

	//	return rsl::make_unique<Map>(rsl::move(map_matrix), rsl::move(map_render_data));
	//}

	TileRenderer* GameSession::init_tile_renderer(const MapRenderData& mapRenderData)
	{
		REX_ASSERT_X(mapRenderData.tileset != nullptr, "No tileset texture provided to the renderer");

		TileRendererDesc tile_renderer_desc{};
		tile_renderer_desc.width_in_tiles = constants::g_screen_width_in_tiles;
		tile_renderer_desc.height_in_tiles = constants::g_screen_height_in_tiles;
		tile_renderer_desc.tileset_texture = mapRenderData.tileset.get();
		tile_renderer_desc.blockset = mapRenderData.blockset.get();

		return rex::gfx::gal::instance()->add_renderer<TileRenderer>(tile_renderer_desc);
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

				m_player_position.x = static_cast<s8>(rsl::clamp_max(static_cast<s32>(m_player_position.x), m_active_map->map_matrix().width_in_tiles() - max_player_pos.x));
				m_player_position.y = static_cast<s8>(rsl::clamp_max(static_cast<s32>(m_player_position.y), m_active_map->map_matrix().height_in_tiles() - max_player_pos.y));

			});
	}
}