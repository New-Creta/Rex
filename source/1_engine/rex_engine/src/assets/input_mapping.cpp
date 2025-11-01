#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/input/key_down.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogInputMapping);

	InputMapping::InputMapping(rsl::unordered_map<rsl::string, InputAction>&& mappings)
		: m_mappings(rsl::move(mappings))
		, m_actions()
	{
		rex::event_system::instance()->subscribe<rex::KeyDown>(
			[this](const rex::KeyDown& ev)
			{
				rex::KeyState input_info{};
				input_info.action.type = rex::InputActionType::Key;
				input_info.action.data.key_code = ev.key();
				input_info.ticks_pressed = ev.num_ticks();
				
				handle_input(input_info);
			});
	}

	void InputMapping::handle_input(const rex::KeyState& inputInfo)
	{
		auto it = m_actions.find(inputInfo.action);
		if (it != m_actions.cend())
		{
			it->value(inputInfo);
		}
	}

	void InputMapping::bind_action(rsl::string_view name, input_action_callback&& callback)
	{
		auto it = m_mappings.find(name);
		if (it == m_mappings.cend())
		{
			REX_ERROR(LogInputMapping, "'{}' is not a known mapping", name);
			return;
		}

		m_actions.emplace(it->value, rsl::move(callback));
	}
}
