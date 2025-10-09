#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogInputMapping);

	InputMapping::InputMapping(rsl::unordered_map<rsl::string, InputAction>&& mappings)
		: m_mappings(rsl::move(mappings))
		, m_actions()
	{

	}

	void InputMapping::handle_input(const rex::InputInfo& inputInfo)
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
