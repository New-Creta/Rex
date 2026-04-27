#pragma once

#include "rex_std/string.h"

namespace rex
{
	class ScriptModule
	{
	public:
		ScriptModule(rsl::string_view assemblyPath);

		rsl::string_view assembly_path() const;

	private:
		rsl::string m_assembly_path;
	};
}