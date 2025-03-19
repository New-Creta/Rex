#pragma once

#include "rex_std/string_view.h"

namespace rex
{
	void open_file(rsl::string_view filepath);
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/platform/win/system/win_open_file.h"
#endif
