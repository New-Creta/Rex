#pragma once

#include "rex_engine/input/key_state.h"

#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/unordered_map.h"

namespace rex
{
	class OsKeyDown;
	class OsKeyUp;

	class InputState
	{
	public:
		InputState();

		void tick();

	private:
		void on_os_key_down(const OsKeyDown& ev);
		void on_os_key_up(const OsKeyUp& ev);
		
	private:
		rsl::unordered_map<KeyCode, s32> m_key_states;
	};
}