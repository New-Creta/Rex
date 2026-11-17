#pragma once

#include "rex_std/string_view.h"

namespace rex
{
	using library_handle = void*;
	using procedure_address = void*;

	library_handle load_library(rsl::string_view path);

	// A procedure is a function within the currently assembly.
	// they're part of the executable or any of the modules loaded into the executable
	// Their API is not always available in a header file, this is the only other way to load them
	procedure_address load_procedure(library_handle lib, rsl::string_view name);
}

#ifdef REX_PLATFORM_WINDOWS
	#include "rex_engine/platform/win/system/win_library_loader.h"
#endif