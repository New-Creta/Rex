#pragma once

#include "rex_engine/serialization/serializer_base.h"

namespace rex
{
	struct MapDesc;
	struct MapHeader;
	struct ObjectEvent;
	enum class ObjectEventType;

	class MapSerializer : public Serializer
	{
	public:
		rsl::unique_ptr<Asset> serialize_from_json(const rex::json::json& jsonContent, LoadFlags loadFlags) override;
		rsl::unique_ptr<Asset> serialize_from_binary(memory::BlobView content) override;

		void hydrate_asset(Asset* asset, const rex::json::json& jsonContent) override;
		void hydrate_asset(Asset* asset, memory::BlobView content) override;

		rex::json::json serialize_to_json(Asset* asset) override;
		rex::memory::Blob serialize_to_binary(Asset* asset) override;

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
		rsl::unique_ptr<ObjectEvent> init_object_event_from_json(const json::json& jsonContent);
		ObjectEventType object_event_type_from_json(const json::json& jsonContent);

	};
}