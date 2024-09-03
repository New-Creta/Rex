#include "pokemon/poke_map.h"

#include "rex_engine/gfx/primitives/box.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_reader.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/resources/texture_2d.h"

#include "glm/glm.hpp"
#include "rex_engine/images/stb_image.h"

#include "rex_engine/components/transform_component.h"
#include "rex_std/bonus/cinttypes.h"
#include "rex_std/limits.h"
#include "rex_engine/text_processing/text_processing.h"

// Game Boy & Game Boy Color Resolution
// 160 x 144
// Tileset: 8x8
// 
// Player is always located at x=10,y=9
// with top left as origin
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   | x |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+


namespace pokemon
{
	struct MapHeader;

	struct connection
	{
		s8 map_id;
		void* blocks;
		MapHeader* map;
		s8 size;
		s8 width;
		s8 y;
		s8 x;
		s16* window;
	};

	struct MapHeader
	{
		s8 tileset_id;
		s8 height;
		s8 width;
		void* blocks;
		void* text;
		void* script;
		s8 connections_bitfield; // bit fields indicating which connections we have
		rsl::vector<connection> connections;
		void* object;
	};

	Map::Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob)
		: m_player_chunk()
		, m_scene(scene)
	{
		m_name.assign(jsonBlob["name"].get<rsl::string_view>());

		m_tileset = load_tileset(jsonBlob["tileset"]);
		m_blockset = load_blockset(jsonBlob["blockset"]);
		m_blocks = load_blocks(jsonBlob["blocks"]);
		m_width = jsonBlob["width"];
		m_height = jsonBlob["height"];
		load_connections(jsonBlob);
	}

	void Map::load_connections(const rex::json::json& jsonBlob)
	{
		if (!jsonBlob.contains("connections"))
		{
			return;
		}

		for (const auto& connection_json : jsonBlob["connections"])
		{
			rsl::unique_ptr<MapConnection> connection = rsl::make_unique<MapConnection>();
			connection->direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
			connection->map = connection_json["map"];
			connection->offset = connection_json["offset"];
			switch (connection->direction)
			{
			case pokemon::Direction::North:		 m_north_connection = rsl::move(connection); break;
			case pokemon::Direction::East:		 m_east_connection  = rsl::move(connection); break;
			case pokemon::Direction::South:		 m_south_connection = rsl::move(connection); break;
			case pokemon::Direction::West:		 m_west_connection  = rsl::move(connection); break;
			}
		}
	}


	MapConnection* Map::north_connection()
	{
		return m_north_connection.get();
	}
	MapConnection* Map::east_connection()
	{
		return m_east_connection.get();
	}
	MapConnection* Map::south_connection()
	{
		return m_south_connection.get();
	}
	MapConnection* Map::west_connection()
	{
		return m_west_connection.get();
	}

	void Map::set_player_chunk(rsl::string_view name)
	{
		// Don't load to a chunk if it's already the active chunk of the player
		if (is_current_player_chunk(name))
		{
			return;
		}

		// If we're in a chunk already, we need to clear all active chunks
		// We might be able to optimize here by only deactiviating the chunks
		// that won't be active when the new chunk is active
		if (m_player_chunk.handle() != entt::null)
		{
			m_active_chunk_components.clear();
		}

		rex::gfx::Entity entity = m_scene->find_entity(name);
		REX_ASSERT_X(entity.handle() != entt::null, "Loading a chunk that is not in the scene. chunk name: {}", name);

		m_player_chunk = entity;
		ChunkComponent* player_chunk = m_active_chunk_components.emplace_back(&m_player_chunk.component<ChunkComponent>());
		for (const ChunkConnection& conn : player_chunk->connections())
		{
			rex::gfx::Entity conn_ent = m_scene->find_entity(conn.dst_chunk_name());
			m_active_chunk_components.push_back(&conn_ent.component<ChunkComponent>());
		}

		// build texture based on tileset and blocket.
		// ..
		// The tileset and blockset is shared for the entire overworld
		// So there's no need for it to actually be stored in the chunk as all this data will be duplicated
		// There are no non-overworld chunks that connect with each other

		//TileSet* tileset = load_tileset(m_player_chunk->tileset());
		//BlockSet* blockset = load_blockset(m_player_chunk->blockset());
		//Blocks* blocks = load_blocks(m_player_chunk->blocks());

 	}

	void Map::load_all_tilesets(rsl::string_view tilesetsPath)
	{
		rsl::vector<rsl::string> files;
		rex::memory::Blob file_blob = rex::vfs::read_file(tilesetsPath);
		rsl::string_view file_content = rex::memory::blob_to_string_view(file_blob);
		rsl::vector<rsl::string_view> lines = rsl::split(file_content, "\n");
		for (rsl::string_view line : lines)
		{
			if (!line.empty())
			{
				line = rex::strip(line);
				rsl::string tileset_path = rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Tilesets), line);
				m_tilesets.push_back(load_tileset(tileset_path));
			}
		}
	}

	void Map::load_all_chunks(const rex::json::json& jsonBlob)
	{
		if (!jsonBlob.contains("chunks"))
		{
			return;
		}
		const rex::json::json& all_chunks_json = jsonBlob["chunks"];

		for (const rex::json::json& chunk_json : all_chunks_json)
		{
			rex::gfx::Entity& entity = m_chunks.emplace_back(m_scene->add_entity(chunk_json["name"]));
			entity.add_component<ChunkComponent>(chunk_json);
		}
	}
	bool Map::is_current_player_chunk(rsl::string_view name) const
	{
		if (m_player_chunk.handle() == entt::null)
		{
			return false;
		}

		if (m_player_chunk.component<ChunkComponent>().name() != name)
		{
			return false;
		}

		return true;
	}

}