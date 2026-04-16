#include "rex_engine/input/input_state.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/input/key_down.h"
#include "rex_engine/event_system/events/input/key_up.h"
#include "rex_engine/event_system/events/input/os_key_down.h"
#include "rex_engine/event_system/events/input/os_key_up.h"

#include "rex_engine/engine/engine.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogInputState);

	InputState::InputState()
	{
		event_system::instance()->subscribe<OSKeyDown>([this](const OSKeyDown& ev) { on_os_key_down(ev); });
		event_system::instance()->subscribe<OSKeyUp>([this](const OSKeyUp& ev) { on_os_key_up(ev); });
	}

	void InputState::tick()
	{
		rsl::unordered_map<KeyCode, s32> temp_key_states = m_key_states;
		for (auto& [key, ticks] : temp_key_states)
		{
			if (ticks >= 0)
			{
				event_system::instance()->enqueue_event(KeyDown(key, ticks));
				m_key_states[key]++;
			}
			else
			{
				event_system::instance()->enqueue_event(KeyUp(key));
				m_key_states.erase(key);
			}
		}
	}

	void InputState::on_os_key_down(const OSKeyDown& ev)
	{
		if (!m_key_states.contains(ev.key()))
		{
			m_key_states[ev.key()] = 0;
		}
	}
	void InputState::on_os_key_up(const OSKeyUp& ev)
	{
		m_key_states[ev.key()] = -1;
	}

}