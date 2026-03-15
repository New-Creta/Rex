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
			rsl::string_view type_str = mapping["type"];
			InputActionType type = rsl::enum_refl::enum_cast<InputActionType>(type_str).value();

			InputAction action { };
			if (mapping.contains("key"))
			{
				rsl::string_view key = mapping["key"];

				action.device = InputDeviceType::Keyboard;
				action.type = type;
				action.data.key_code = rsl::enum_refl::enum_cast<KeyCode>(key).value();
			}
			else if (mapping.contains("button"))
			{
				rsl::string_view button = mapping["button"];
				action.device = InputDeviceType::Mouse;
				action.type = type;
				action.data.mouse_button = rsl::enum_refl::enum_cast<MouseButton>(button).value();
			}

			mappings.emplace(name, action);
		}

		return rsl::make_unique<InputMapping>(rsl::move(mappings));
	}
}