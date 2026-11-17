#include "rex_engine/engine/module.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/system/process.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogModule);

	Module::Module(rsl::string_view moduleName, rsl::string_view configName, rsl::string_view sourceRoot, rsl::string_view dataRoot, rsl::string_view targetPath, rsl::vector<Module*>&& dependencies)
		: m_name(moduleName)
		, m_config_name(configName)
		, m_source_root(sourceRoot)
		, m_data_path(dataRoot)
		, m_target_path(targetPath)
		, m_dependencies(rsl::move(dependencies))
		, m_runtime_dependencies()
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

	void Module::add_runtime_dependency(Module* module)
	{
		m_runtime_dependencies.push_back(module);
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