#include "rex_engine/engine/script_module.h"

namespace rex
{
	ScriptModule::ScriptModule(rsl::string_view assemblyPath)
		: m_assembly_path(assemblyPath)
	{
	}

	rsl::string_view ScriptModule::assembly_path() const
	{
		return m_assembly_path;
	}

}