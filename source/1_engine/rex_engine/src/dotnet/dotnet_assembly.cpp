#include "rex_engine/dotnet/dotnet_assembly.h"

namespace rex
{
	DotNetAssembly::DotNetAssembly(rsl::string_view assemblyPath)
		: m_assembly_path(assemblyPath)
	{
		m_set_function = get_function<set_function_fn>("DotNetLib2.DelegateSetter", "SetDelegate");
	}

	void DotNetAssembly::set_function(rsl::string_view classPath, rsl::string_view delegateName, void* func)
	{
		m_set_function(classPath.data(), delegateName.data(), func);
	}
}