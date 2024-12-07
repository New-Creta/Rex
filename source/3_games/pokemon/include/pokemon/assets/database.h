#pragma once

#include "rex_std/memory.h"

#include "rex_engine/text_processing/json.h"

#include "pokemon/poke_structs.h"

namespace pokemon
{
	namespace asset_db
	{
		MapData* find_map(rsl::string_view mapPath);
		MapData* find_map_without_connections(rsl::string_view mapPath);
		const rsl::unique_array<u8>& find_map_blocks(const MapHeader& mapHeader);

		MapHeader load_map_header(const rex::json::json& jsonBlob);
		MapHeader load_map_header(rsl::string_view mapPath);

		void load_map_data(MapData* mapObject, const rex::json::json& jsonBlob);
		rsl::unique_ptr<MapData> load_map_data(rsl::string_view mapPath);
		rsl::unique_ptr<MapData> load_map_header_only(rsl::string_view mapPath);
	}
}