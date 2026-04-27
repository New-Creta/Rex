#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/string_view.h"

namespace rex
{
	struct CSharpScriptEntryPoint
	{
		rsl::string_view classPath;
		rsl::string_view function;
	};

	class CSharpScript
	{
	public:
		CSharpScript(rsl::string_view assembly, CSharpScriptEntryPoint entryPoint);
		void invoke(void* ptr = nullptr, s32 size = 0);
		
	private:
		using entrypoint_fn = void(*)(void*, s32);
		entrypoint_fn m_entrypoint_fn;
	};
}