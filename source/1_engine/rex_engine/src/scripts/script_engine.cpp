#include "rex_engine/scripts/script_engine.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/engine/module_manager.h"
#include "rex_engine/system/process.h"

#include "rex_engine/dotnet/dotnet_bridge.h"

namespace rex
{
	ScriptEngine::ScriptEngine()
	{
		init_dotnet();
	}
	
	rsl::unique_ptr<CSharpScript> ScriptEngine::load_script(rsl::string_view name, CSharpScriptEntryPoint entryPoint)
	{
		const Module* script_module = module_manager::instance()->current()->find_runtime_dependency(name);
		CSharpScript::entrypoint_fn entrypoint_func = m_dotnet_bridge->load_function<CSharpScript::entrypoint_fn>(script_module->target_path(), entryPoint.classPath, entryPoint.function);

		return rsl::make_unique<CSharpScript>(entrypoint_func);
	}

	bool ScriptEngine::compile_script(rsl::string_view name)
	{
		RunProcessResult result = module_manager::instance()->compile_module(name, REX_CONFIG_NAME);
		return result.returncode == 0;
	}

	void ScriptEngine::init_dotnet()
	{
		rsl::string_view dotnet_runtime_config_path = path::join(vfs::instance()->mount_path(MountingPoint::EngineRoot), "dotnet", "dotnet.runtimeconfig.json");
		m_dotnet_bridge = rsl::make_unique<DotNetBridge>(dotnet_runtime_config_path);
	}

	namespace script_engine
	{
		globals::GlobalUniquePtr<ScriptEngine> g_script_engine;
		void init(globals::GlobalUniquePtr<ScriptEngine> scriptEngine)
		{
			g_script_engine = rsl::move(scriptEngine);
		}
		ScriptEngine* instance()
		{
			return g_script_engine.get();
		}
		void shutdown()
		{
			g_script_engine.reset();
		}
	}
}