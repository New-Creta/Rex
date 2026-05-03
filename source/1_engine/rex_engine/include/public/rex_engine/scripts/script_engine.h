#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/scripts/csharp_script.h"

#include "rex_std/unordered_map.h"

namespace rex
{
	class DotNetBridge;

	class ScriptEngine
	{
	public:
		ScriptEngine();

		rsl::unique_ptr<CSharpScript> load_script(rsl::string_view name, CSharpScriptEntryPoint entryPoint);

	private:
		void init_dotnet();

		bool compile_script(rsl::string_view name);

	private:
		rsl::unique_ptr<DotNetBridge> m_dotnet_bridge;

		bool m_allow_runtime_caching;
	};

	namespace script_engine
	{
		void init(globals::GlobalUniquePtr<ScriptEngine> scriptEngine);
		ScriptEngine* instance();
		void shutdown();
	}
}