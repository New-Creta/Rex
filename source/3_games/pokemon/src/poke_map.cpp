#include "pokemon/poke_map.h"

#include "rex_renderer_core/primitives/box.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "glm/glm.hpp"
#include "rex_engine/images/stb_image.h"

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
	Map::Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob)
		: m_current_chunk_info(nullptr)
		, m_scene(scene)
	{
		m_name.assign(jsonBlob["name"].get<rsl::string_view>());

		// Load in all the information of every chunk so we can access it later
		const rex::json::json& chunks_json = jsonBlob["chunks"];
		for (const rex::json::json& chunk_json : chunks_json)
		{
			ChunkInfo chunk_info{};
			chunk_info.name.assign(chunk_json["name"].get<rsl::string_view>());
			chunk_info.background.assign(chunk_json["background"].get<rsl::string_view>());
			chunk_info.connections = chunk_json["connections"].get<rsl::vector<rsl::string>>();
			chunk_info.pos.x = chunk_json["position"]["x"].get<f32>();
			chunk_info.pos.y = chunk_json["position"]["y"].get<f32>();
			chunk_info.pos.z = chunk_json["position"]["z"].get<f32>();

			m_level_infos.emplace(chunk_info.name, chunk_info);
		}
	}

	void Map::load_chunk(rsl::string_view name)
	{
		// Don't load to a chunk if it's already the active chunk
		if (m_current_chunk_info && m_current_chunk_info->name == name)
		{
			return;
		}

		// If we're in an existing chunk all active chunks
		if (m_current_chunk_info)
		{
			m_active_chunks.clear();
		}

		// Load the targetted chunk's info and start loading it
		const ChunkInfo& chunk_info = m_level_infos.at(name);
		m_active_chunks.emplace_back(m_scene->add_entity(), chunk_info.name, chunk_info.background, chunk_info.pos);
		
		for (const auto& connection : chunk_info.connections)
		{
			const ChunkInfo& connection_info = m_level_infos.at(connection);
			m_active_chunks.emplace_back(m_scene->add_entity(), connection_info.name, connection_info.background, connection_info.pos);
		}

		m_current_chunk_info = &chunk_info;
	}
}