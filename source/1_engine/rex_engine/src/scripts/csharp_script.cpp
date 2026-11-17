#include "rex_engine/scripts/csharp_script.h"

#include "rex_engine/dotnet/dotnet_bridge.h"

namespace rex
{
	CSharpScript::CSharpScript(entrypoint_fn func)
		: m_entrypoint_fn(func)
	{
	}

	void CSharpScript::invoke(void* data, s32 size)
	{
		m_entrypoint_fn(data, size);
	}


}
