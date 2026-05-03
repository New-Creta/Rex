#include "rex_engine/engine/module_manager.h"

#include "rex_engine/system/process.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_engine/ninja/ninja.h"

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

	void ModuleManager::compile_module(rsl::string_view name, rsl::string_view config)
	{
		scratch_string rex_python_path = path::join(engine::instance()->root(), "_rex.py");

		auto build_module_cmd = rsl::format("py {} build -project={} -config={} -dont_build_dependencies", rex_python_path, name, config);
		process::run(build_module_cmd);
	}

	Module* ModuleManager::init_module(rsl::string_view modulePath)
	{
		if (!vfs::instance()->exists(modulePath))
		{
			return nullptr;
		}

		// If the modules was already loaded, use that one instead
		rex::json::json json_content = rex::json::read_from_file(modulePath);
		rsl::string_view module_name = json_content["module_name"];
		
		auto it = rsl::find_if(m_all_modules.begin(), m_all_modules.end(),
			[&](const rsl::unique_ptr<Module>& module)
			{
				return module->name() == module_name;
			});
		if (it != m_all_modules.end())
		{
			return it->get();
		}
		
		rsl::string_view source_root = json_content["source_root"];
		rsl::string_view data_root = json_content["data_root"];
		rsl::string_view config_name = json_content["config_name"];
		rsl::string_view target_path = json_content["target_path"];

		// We can have an infinite loop / stack overflow here as a module have a runtime dependency
		// which in turn can have a build dependency on the original module
		// that's why we load runtime modules independantly, this also makes more sense
		// if runtime dependencies aren't necessarily known at init time
		rsl::vector<Module*> dependency_ptrs = read_dependencies(json_content, "build_dependencies");
		rsl::unique_ptr<Module> new_module = rsl::make_unique<Module>(module_name, config_name, source_root, data_root, target_path, rsl::move(dependency_ptrs));
		Module* raw_new_module = m_all_modules.emplace_back(rsl::move(new_module)).get();

		rsl::vector<Module*> runtime_dependency_ptrs = read_dependencies(json_content, "runtime_dependencies");
		for (Module* runtime_dependency : runtime_dependency_ptrs)
		{
			raw_new_module->add_runtime_dependency(runtime_dependency);
		}

		return raw_new_module;
	}

	// read dependencies from a json blob and return it
	rsl::vector<Module*> ModuleManager::read_dependencies(json::json& jsonBlob, rsl::string_view fieldName)
	{
		const rex::json::json& dependencies = jsonBlob[fieldName];
		rsl::vector<Module*> dependency_ptrs;
		for (rsl::string_view dependency : dependencies)
		{
			Module* module = init_module(dependency);
			if (module)
			{
				dependency_ptrs.push_back(module);
			}
		}

		return dependency_ptrs;
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