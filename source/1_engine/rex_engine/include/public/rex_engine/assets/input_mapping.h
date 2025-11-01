#pragma once

#include "rex_engine/assets/asset.h"

#include "rex_engine/input/input_action.h"
#include "rex_engine/input/key_state.h"

#include "rex_std/functional.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"

namespace rex
{
	class InputMapping : public Asset
	{
	public:
		using input_action_callback = rsl::function<void(const KeyState&)>;
		InputMapping(rsl::unordered_map<rsl::string, InputAction>&& mappings);

		void handle_input(const KeyState& inputInfo);

		void bind_action(rsl::string_view name, input_action_callback&& callback);

	private:
		rsl::unordered_map<rsl::string, InputAction> m_mappings;
		rsl::unordered_map<InputAction, input_action_callback> m_actions;
	};
}