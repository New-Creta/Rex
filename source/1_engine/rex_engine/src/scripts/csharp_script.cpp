#include "rex_engine/scripts/csharp_script.h"

#include "rex_engine/dotnet/dotnet_bridge.h"

namespace rex
{
	CSharpScript::CSharpScript(rsl::string_view assembly, CSharpScriptEntryPoint entryPoint)
	{
		//m_entrypoint_fn = dotnet::instance()->load_function<entrypoint_fn>(assembly, entryPoint.classPath, entryPoint.function);
	}

	void CSharpScript::invoke(void* data, s32 size)
	{
		m_entrypoint_fn(data, size);
	}


}
