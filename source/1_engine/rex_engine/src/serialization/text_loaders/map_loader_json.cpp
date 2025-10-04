#include "rex_engine/serialization/text_loaders/map_loader_json.h"

#include "rex_engine/assets/map.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/text_processing/json.h"

#include "rex_engine/assets/map_objects/map_pickup.h"
#include "rex_engine/assets/map_objects/map_pokemon.h"
#include "rex_engine/assets/map_objects/map_textboard.h"
#include "rex_engine/assets/map_objects/map_trainer.h"
#include "rex_engine/assets/map_objects/map_warp.h"

#include "rex_engine/string/stringid.h"

namespace rex
{
	MapLoaderJson::MapLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			})
	{}

	rsl::unique_ptr<Asset> MapLoaderJson::load(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		rex::json::json json_content = rex::json::read_from_file(assetPath);

		MapDesc map_desc{};

		init_map_header(json_content, map_desc);
		init_blocks(json_content, map_desc);
		init_blockset(json_content, map_desc);

		if (!rsl::has_flag(loadFlags, LoadFlags::PartialLoad))
		{
			hydrate_desc(json_content, map_desc);
		}

		return rsl::make_unique<Map>(rsl::move(map_desc));
	}
	void MapLoaderJson::hydrate_asset(Asset* asset, rsl::string_view assetPath)
	{
		rex::json::json json_content = rex::json::read_from_file(assetPath);

		// Destroy the map at the location provided
		Map* map = static_cast<Map*>(asset);
		rsl::destroy_at(map);

		// Initialize the map
		MapDesc map_desc{};
		init_map_header(json_content, map_desc);
		init_blocks(json_content, map_desc);
		init_blockset(json_content, map_desc);

		hydrate_desc(json_content, map_desc);

		// Construct a new map object at the old asset's location
		rsl::construct_at(map, rsl::move(map_desc));
	}

	void MapLoaderJson::hydrate_desc(const json::json& jsonContent, MapDesc& desc)
	{
		init_connections(jsonContent, desc);
		init_objects(jsonContent, desc);
		init_object_events(jsonContent, desc);
		init_warps(jsonContent, desc);
		init_text_events(jsonContent, desc);
		init_scripts(jsonContent, desc);
	}

	void MapLoaderJson::init_map_header(const json::json& jsonContent, MapDesc& desc)
	{
		desc.map_header = load_map_header_from_json(jsonContent);
	}
	void MapLoaderJson::init_connections(const json::json& jsonContent, MapDesc& desc)
	{
		desc.connections = rsl::make_unique<MapConnection[]>(jsonContent["connections"].size());
		s32 idx = 0;
		for (const json::json& conn : jsonContent["connections"])
		{
			MapConnection& connection = desc.connections[idx];
			connection.direction = rsl::enum_refl::enum_cast<Direction>(conn["direction"].get<rsl::string_view>()).value();
			connection.offset = conn["offset"]; // is in squares (2x2 tiles)
			connection.map = asset_db::instance()->load<Map>(conn["map"], LoadFlags::PartialLoad);
			++idx;
		}
	}
	void MapLoaderJson::init_objects(const json::json& jsonContent, MapDesc& desc)
	{
		REX_UNUSED_PARAM(jsonContent);
		REX_UNUSED_PARAM(desc);

		//s32 idx = 0;
		//desc.objects = rsl::make_unique<MapObject[]>(jsonContent["objects"].size());
		//for (const json::json& obj : jsonContent["objects"])
		//{
		//	desc.objects[idx]->name = obj["name"];
		//	++idx;
		//}
	}
	void MapLoaderJson::init_object_events(const json::json& jsonContent, MapDesc& desc)
	{
		REX_UNUSED_PARAM(jsonContent);
		REX_UNUSED_PARAM(desc);

		//s32 idx = 0;
		//desc.object_events = rsl::make_unique<rsl::unique_ptr<ObjectEvent>[]>(jsonContent["object_events"].size());
		//for (const json::json& evt : jsonContent["object_events"])
		//{
		//	desc.object_events[idx] = init_object_event_from_json(evt);
		//	++idx;
		//}

	}
	void MapLoaderJson::init_warps(const json::json& jsonContent, MapDesc& desc)
	{
		s32 idx = 0;
		desc.warps = rsl::make_unique<WarpEvent[]>(jsonContent["warps"].size());
		for (const json::json& warp : jsonContent["warps"])
		{
			desc.warps[idx].pos.x = warp["x"];
			desc.warps[idx].pos.y = warp["y"];
			desc.warps[idx].dst_map_id = warp["dst_map_id"];
			desc.warps[idx].dst_warp_id = warp["dst_warp_id"];
			++idx;
		}
	}
	void MapLoaderJson::init_text_events(const json::json& jsonContent, MapDesc& desc)
	{
		s32 idx = 0;
		desc.text_events = rsl::make_unique<TextEvent[]>(jsonContent["bg_events"].size());
		for (const json::json& evt : jsonContent["bg_events"])
		{
			desc.text_events[idx].pos.x = evt["x"];
			desc.text_events[idx].pos.y = evt["y"];
			desc.text_events[idx].text = evt["text"];
			desc.text_events[idx].sign_id = -1;
			++idx;
		}
	}
	void MapLoaderJson::init_scripts(const json::json& jsonContent, MapDesc& desc)
	{
		s32 idx = 0;
		desc.scripts = rsl::make_unique<rsl::string[]>(jsonContent["scripts"].size());
		for (const json::json& script : jsonContent["scripts"])
		{
			desc.scripts[idx] = script;
			++idx;
		}
	}
	void MapLoaderJson::init_blocks(const json::json& jsonContent, MapDesc& desc)
	{
		memory::Blob blockmap = vfs::instance()->read_file(jsonContent["map_blocks"]);
		desc.blocks = rsl::make_unique<u8[]>(desc.map_header.width_in_blocks * desc.map_header.height_in_blocks);
		rsl::memcpy(desc.blocks.get(), blockmap.data(), blockmap.size());
	}
	void MapLoaderJson::init_blockset(const json::json& jsonContent, MapDesc& desc)
	{
		desc.blockset = asset_db::instance()->load<Blockset>(jsonContent["blockset"]);
	}

	MapHeader MapLoaderJson::load_map_header_from_json(const json::json& jsonContent)
	{
		MapHeader header{};

		header.name = jsonContent["name"];
		header.width_in_blocks = jsonContent["width"];
		header.height_in_blocks = jsonContent["height"];
		header.border_block_idx = jsonContent["border_block_idx"];

		return header;
	}
	rsl::unique_ptr<MapObject> MapLoaderJson::init_object_event_from_json(const json::json& jsonContent)
	{
		rsl::unique_ptr<MapObject> res;
		MapObjectType obj_evt_type = object_event_type_from_json(jsonContent);
		switch (obj_evt_type)
		{
		case MapObjectType::Item:
		{
			rsl::unique_ptr<MapPickup> item_evt = rsl::make_unique<MapPickup>();
			item_evt->item = jsonContent["item"];
			res = rsl::move(item_evt);
			break;
		}
		case MapObjectType::Trainer:
		{
			rsl::unique_ptr<MapTrainer> trainer_evt = rsl::make_unique<MapTrainer>();
			trainer_evt->trainer_class = jsonContent["trainer_class"];
			trainer_evt->trainer_number = jsonContent["trainer_number"];
			res = rsl::move(trainer_evt);
			break;
		}
		case MapObjectType::Pokemon:
		{
			rsl::unique_ptr<MapPokemon> pokemon_evt = rsl::make_unique<MapPokemon>();
			pokemon_evt->pokemon_id = jsonContent["pokemon_id"];
			pokemon_evt->pokemon_level = jsonContent["pokemon_level"];
			res = rsl::move(pokemon_evt);
			break;
		}
		case MapObjectType::Character:
			rsl::unique_ptr<MapCharacter> char_evt = rsl::make_unique<MapCharacter>();
			char_evt->movement = jsonContent["movement"];
			char_evt->direction = jsonContent["direction"];
			res = rsl::move(char_evt);
		}

		res->pos.x = jsonContent["x"];
		res->pos.y = jsonContent["y"];
		res->sprite_id = jsonContent["sprite"];
		res->text_id = jsonContent["text"];

		return res;
	}
	MapObjectType MapLoaderJson::object_event_type_from_json(const json::json& jsonContent)
	{
		if (jsonContent.contains("item"))
		{
			return MapObjectType::Item;
		}

		if (jsonContent.contains("trainer_class"))
		{
			return MapObjectType::Trainer;
		}

		if (jsonContent.contains("pokemon_id"))
		{
			return MapObjectType::Pokemon;
		}

		return MapObjectType::Character;
	}

}