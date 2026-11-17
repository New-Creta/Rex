#include "rex_engine/platform/win/system/win_library_loader.h"

#include <Windows.h>

namespace rex
{
	library_handle load_library(rsl::string_view path)
	{
		return LoadLibraryA(path.data());
	}
	procedure_address load_procedure(library_handle lib, rsl::string_view name)
	{
		return GetProcAddress((HMODULE)lib, name.data());
	}
}