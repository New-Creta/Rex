#include "rex_engine/engine/module_manager.h"

#include "rex_engine/system/process.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_std/bonus/algorithms.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogModuleManager);

	ModuleManager::ModuleManager()
		: m_current_module(nullptr)
	{
		scratch_string module_dir = path::find_in_parent("module.json", path::parent_path(current_process::path()));
		if (module_dir.empty())
		{
			REX_WARN(LogModuleManager, "Cannot find current module's path {}", module_dir);
			return;
		}

		m_current_module = init_module(module_dir);
	}

	const Module* ModuleManager::current() const
	{
		return m_current_module;
	}

	rsl::string_view ModuleManager::script_module_path(rsl::string_view moduleName)
	{
		auto it = m_all_script_modules.find(moduleName);
		if (it != m_all_script_modules.cend())
		{
			return it->value->assembly_path();
		}

		return "";
	}

	Module* ModuleManager::init_module(rsl::string_view modulePath)
	{
		if (!vfs::instance()->exists(modulePath))
		{
			return nullptr;
		}

		// Example module content
		// {
		//   "name": "Bob",
		//   "data_path": "D:\MyData",
		//   "dependencies": [
		//	   "D:\module_a\module.json",
		//	   "D:\module_b\module.json",
		//	  ]
		// }
		//

		// If the modules was already loaded, use that one instead
		rex::json::json json_content = rex::json::read_from_file(modulePath);
		rsl::string_view name = json_content["name"];
		auto it = rsl::find_if(m_all_modules.begin(), m_all_modules.end(),
			[&](const rsl::unique_ptr<Module>& module)
			{
				return module->name() == name;
			});
		
		if (it != m_all_modules.end())
		{
			return it->get();
		}

		rsl::string_view data_path = json_content["data_path"];
		const rex::json::json& dependencies = json_content["dependencies"];
		rsl::vector<Module*> dependency_ptrs;
		for (rsl::string_view dependency : dependencies)
		{
			Module* module = init_module(dependency);
			if (module)
			{
				dependency_ptrs.push_back(module);
			}
		}

		const rex::json::json& scripts = json_content["scripts"];
		for (rsl::string_view script : scripts)
		{
			rsl::string_view script_name = path::stem(script);
			auto it = m_all_script_modules.find(script_name);
			if (it == m_all_script_modules.cend())
			{
				m_all_script_modules.emplace(script_name, rsl::make_unique<ScriptModule>(script));
			}
		}

		m_all_modules.emplace_back(rsl::make_unique<Module>(name, data_path, rsl::move(dependency_ptrs)));

		return m_all_modules.back().get();
	}

	namespace module_manager
	{
		globals::GlobalUniquePtr<ModuleManager> g_module_manager;
		void init(globals::GlobalUniquePtr<ModuleManager> moduleManager)
		{
			g_module_manager = rsl::move(moduleManager);
		}
		ModuleManager* instance()
		{
			return g_module_manager.get();
		}
		void shutdown()
		{
			g_module_manager.reset();
		}
	}
}