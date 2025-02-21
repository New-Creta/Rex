#include "pokemon/assets/database.h"

#include "rex_std/unordered_map.h"

#include "rex_engine/engine/types.h"

#include "pokemon/poke_map.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_engine/text_processing/text_processing.h"

namespace pokemon
{
	namespace asset_db
	{
		// This holds the information about the map, width, height, blockset to use, ...
		rsl::unordered_map<rsl::string_view, rsl::unique_ptr<MapData>> g_maps;

		// This holds the map blcoks themselves, as in the block indices themselves
		rsl::unordered_map<rsl::string_view, rsl::unique_array<u8>> g_map_blocks;

		rsl::unordered_map<rsl::string_view, MapRenderData> g_map_render_data;

		const rsl::unique_array<u8>& find_map_blocks(const MapHeader& mapHeader)
		{
			if (!g_map_blocks.contains(mapHeader.map_blocks_filepath))
			{
				rex::memory::Blob file_content = rex::vfs::read_file(mapHeader.map_blocks_filepath);
				return g_map_blocks.emplace(mapHeader.map_blocks_filepath, rsl::unique_array<u8>(file_content.release_as_array<u8>())).inserted_element->value;
			}

			return g_map_blocks.at(mapHeader.map_blocks_filepath);
		}

		MapData* find_map(rsl::string_view mapPath)
		{
			if (!g_maps.contains(mapPath))
			{
				rsl::unique_ptr<MapData> map_data = load_map_data(mapPath);

				// If the map wasn't fully loaded (eg. it was loaded as a connection)
				// We need to load it fully now
				if (!map_data->fully_loaded)
				{
					rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
					rex::json::json json_blob = rex::json::parse(file_content);
					load_map_data(map_data.get(), json_blob);
				}
				return g_maps.emplace(mapPath, rsl::move(map_data)).inserted_element->value.get();
			}

			return g_maps.at(mapPath).get();
		}
		MapData* find_map_without_connections(rsl::string_view mapPath)
		{
			if (!g_maps.contains(mapPath))
			{
				rsl::unique_ptr<MapData> map_object = load_map_header_only(mapPath);
				map_object->fully_loaded = false;
				return g_maps.emplace(mapPath, rsl::move(map_object)).inserted_element->value.get();
			}

			return g_maps.at(mapPath).get();
		}

		MapHeader load_map_header(const rex::json::json& jsonBlob)
		{
			MapHeader map_header{};

			map_header.name = jsonBlob["name"];
			map_header.map_blocks_filepath = jsonBlob["map_blocks"];
			map_header.map_render_data_filepath = jsonBlob["tileset"];
			map_header.width = jsonBlob["width"];
			map_header.height = jsonBlob["height"];
			map_header.border_block_idx = jsonBlob["border_block_idx"];

			return map_header;
		}
		MapHeader load_map_header(rsl::string_view mapPath)
		{
			MapHeader map_header{};

			rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
			rex::json::json json_blob = rex::json::parse(file_content);

			return load_map_header(json_blob);
		}

		void load_map_data(MapData* mapObject, const rex::json::json& jsonBlob)
		{
			// Connections
			if (jsonBlob.contains("connections"))
			{
				mapObject->connections.reserve(jsonBlob["connections"].size());
				for (const auto& connection_json : jsonBlob["connections"])
				{
					MapConnection& connection = mapObject->connections.emplace_back();
					connection.direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
					connection.map = find_map_without_connections(connection_json["map"]);
					connection.offset = connection_json["offset"]; // is in squares (2x2 tiles)
				}
			}

			// Wrap events
			if (jsonBlob.contains("wraps"))
			{
				mapObject->wrap_events.reserve(jsonBlob["wraps"].size());
				for (const auto& wrap_json : jsonBlob["wraps"])
				{
					WrapEvent& wrap_event = mapObject->wrap_events.emplace_back();
					wrap_event.x = wrap_json["x"];
					wrap_event.y = wrap_json["y"];
					wrap_event.dst_map_id = wrap_json["dst_map_id"];
					wrap_event.dst_warp_id = wrap_json["dst_warp_id"];
				}
			}

			mapObject->fully_loaded = true;
		}

		rsl::unique_ptr<MapData> load_map_data(rsl::string_view mapPath)
		{
			rex::json::json json_blob = rex::json::read_from_file(mapPath);

			rsl::unique_ptr<MapData> map_data = rsl::make_unique<MapData>();
			map_data->map_header = load_map_header(json_blob);

			load_map_data(map_data.get(), json_blob);

			return map_data;
		}
		rsl::unique_ptr<MapData> load_map_header_only(rsl::string_view mapPath)
		{
			rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
			rex::json::json json_blob = rex::json::parse(file_content);

			REX_ASSERT_X(!json_blob.is_discarded(), "Invalid map json file. {}", rex::quoted(mapPath))
				rsl::unique_ptr<MapData> map_object = rsl::make_unique<MapData>();
			map_object->map_header = load_map_header(json_blob);

			return map_object;
		}

	}
}