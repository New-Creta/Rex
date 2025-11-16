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

		// Handle an input event
		// note we do not differentiate between key up and down events
		// the user should specify their mappings to be triggered on key up or down events
		// and assign them to a binding
		// example:
		// input_mapping->bind_action("my_key_pressed", do_something_on_key_press());
		// input_mapping->bind_action("my_key_release", do_something_on_key_release());
		void handle_input(const KeyState& inputInfo);

		// bind an action to this input mapping
		// as each input mapping registers themselves to listen to key events
		// when an input event is fired that has an action bound to it
		// that action will automatically be called
		void bind_action(rsl::string_view name, input_action_callback&& callback);

	private:
		rsl::unordered_map<rsl::string, InputAction> m_mappings;
		rsl::unordered_map<InputAction, input_action_callback> m_actions;
	};
}