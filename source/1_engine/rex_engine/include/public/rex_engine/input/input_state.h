#pragma once

#include "rex_engine/input/key_state.h"

#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/unordered_map.h"

namespace rex
{
	class OSKeyDown;
	class OSKeyUp;

	// This class holds the input state of all keys of a keyboard
	// it has capacity to tick, incrementing tick count how long keys are pressed
	class InputState
	{
	public:
		InputState();

		// Increase tick count how long keys are pressed
		// remove any keys that are no longer pressed
		void tick();

	private:
		void on_os_key_down(const OSKeyDown& ev);
		void on_os_key_up(const OSKeyUp& ev);
		
	private:
		rsl::unordered_map<KeyCode, s32> m_key_states;
	};
}