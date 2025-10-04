#pragma once

#include "rex_engine/serialization/asset_loader.h"
#include "rex_engine/text_processing/json.h"

namespace rex
{
	struct MapDesc;
	struct MapHeader;
	struct MapObject;
	enum class MapObjectType;

	class MapLoaderJson : public AssetLoader
	{
	public:
		MapLoaderJson();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) override;
		void hydrate_asset(Asset* asset, rsl::string_view assetPath) override;

	private:
		void hydrate_desc(const json::json& jsonContent, MapDesc& desc);
		void init_map_header(const json::json& jsonContent, MapDesc& desc);
		void init_connections(const json::json& jsonContent, MapDesc& desc);
		void init_objects(const json::json& jsonContent, MapDesc& desc);
		void init_object_events(const json::json& jsonContent, MapDesc& desc);
		void init_warps(const json::json& jsonContent, MapDesc& desc);
		void init_text_events(const json::json& jsonContent, MapDesc& desc);
		void init_scripts(const json::json& jsonContent, MapDesc& desc);

		void init_blocks(const json::json& jsonContent, MapDesc& desc);
		void init_blockset(const json::json& jsonContent, MapDesc& desc);

		MapHeader load_map_header_from_json(const json::json& jsonContent);
		rsl::unique_ptr<MapObject> init_object_event_from_json(const json::json& jsonContent);
		MapObjectType object_event_type_from_json(const json::json& jsonContent);

	};
}