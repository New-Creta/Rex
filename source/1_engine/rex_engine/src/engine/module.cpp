#include "rex_engine/engine/module.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/system/process.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogModule);

	Module::Module(rsl::string_view name, rsl::string_view dataPath, rsl::vector<Module*>&& dependencies, rsl::vector<Module*>&& runtimeDependencies)
		: m_name(name)
		, m_data_path(dataPath)
		, m_dependencies(rsl::move(dependencies))
		, m_runtime_dependencies(rsl::move(runtimeDependencies))
	{
	}

	rsl::string_view Module::name() const
	{
		return m_name;
	}
	rsl::string_view Module::data_path() const
	{
		return m_data_path;
	}
	rsl::string_view Module::target_path() const
	{
		return m_target_path;
	}

	const Module* Module::find_runtime_dependency(rsl::string_view name) const
	{
		auto it = rsl::find_if(m_runtime_dependencies.cbegin(), m_runtime_dependencies.cend(),
			[&](const Module* module)
			{
				return module->name() == name;
			});

		return it == m_runtime_dependencies.cend()
			? nullptr
			: *it;
	}

	const rsl::vector<Module*>& Module::dependencies() const
	{
		return m_dependencies;
	}
}