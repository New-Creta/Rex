#include "rex_engine/serialization/text_loaders/input_mapping_loader_json.h"

#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/json.h"

#include "rex_engine/input/input_action.h"

#include "rex_std/bonus/utility/enum_reflection.h"

namespace rex
{
	InputMappingLoaderJson::InputMappingLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			})
	{

	}

	rsl::unique_ptr<Asset> InputMappingLoaderJson::load(rsl::string_view assetPath, LoadFlags flags)
	{
		rex::json::json json_content = rex::json::read_from_file(assetPath);

		if (json_content.is_discarded())
		{
			REX_ERROR(LogAssetLoader, "Could not load InputMapping at '{}'. Json is invalid", json_content);
			return nullptr;
		}

		rsl::unordered_map<rsl::string, InputAction> mappings;
		for (const rex::json::json& mapping : json_content)
		{
			rsl::string_view name = mapping["name"];
			rsl::string_view key = mapping["key"];

			InputAction action { };
			if (key.contains("mouse"))
			{
				action.type = InputActionType::Mouse;
				action.data.mouse_button = rsl::enum_refl::enum_cast<MouseButton>(key).value();
			}
			else
			{
				action.type = InputActionType::Key;
				action.data.key_code = rsl::enum_refl::enum_cast<KeyCode>(key).value();
			}

			mappings.emplace(name, action);
		}

		return rsl::make_unique<InputMapping>(rsl::move(mappings));
	}
}