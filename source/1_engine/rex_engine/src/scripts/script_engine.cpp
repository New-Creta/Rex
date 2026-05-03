#include "rex_engine/scripts/script_engine.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/engine/module_manager.h"
#include "rex_engine/system/process.h"

#include "rex_engine/dotnet/dotnet_bridge.h"

namespace rex
{
	ScriptEngine::ScriptEngine()
		: m_msbuild_path(find_msbuild_path())
	{
		init_dotnet();
	}
	
	rsl::unique_ptr<CSharpScript> ScriptEngine::load_script(rsl::string_view name)
	{
		//rsl::string_view script_path = find_script_path(name);
		//if (vfs::instance()->exists(script_path))
		//{
		//	return rsl::make_unique<CSharpScript>(script_path, CSharpScriptEntryPoint{ "", "" });
		//}

		//if (m_allow_runtime_caching)
		//{
		//	if (compile_script(name))
		//	{
		//		return rsl::make_unique<CSharpScript>(script_path, CSharpScriptEntryPoint{ "", "" });
		//	}
		//}

		return nullptr;
	}

	bool ScriptEngine::compile_script(rsl::string_view name)
	{
		rsl::string_view script_path = name;
		auto arguments = rsl::format("-property:configuration={} -t:restore, rebuild", REX_CONFIG_NAME);
		auto compile_cs_script_cmd = rsl::format("{} {} {}", m_msbuild_path, script_path, arguments);

		return system(compile_cs_script_cmd.c_str()) == 0;
	}

	//rsl::string_view ScriptEngine::find_script_path(rsl::string_view name)
	//{
	//	rsl::string_view script_module_path = module_manager::instance()->script_module_path(name);
	//	if (!script_module_path.empty())
	//	{
	//		return script_module_path;
	//	}

	//	return "";
	//}

	void ScriptEngine::init_dotnet()
	{
		rsl::string_view dotnet_runtime_config_path = path::join(vfs::instance()->mount_path(MountingPoint::EngineRoot), "dotnet", "dotnet.runtimeconfig.json");
		m_dotnet_bridge = rsl::make_unique<DotNetBridge>(dotnet_runtime_config_path);
	}

	rsl::string ScriptEngine::find_msbuild_path()
	{
		rsl::string_view vswhere_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe";
		auto cmd = rsl::format("\"{}\" -latest -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe", vswhere_path);

		RunProcessResult find_msbuild_result = process::run(cmd);
		rsl::string_view vswhere_output = find_msbuild_result.output;
		vswhere_output = strip(vswhere_output);
		
		return rsl::string(vswhere_output);
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