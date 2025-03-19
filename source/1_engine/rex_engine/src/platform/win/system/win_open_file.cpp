#include "rex_engine/platform/win/system/win_open_file.h"

#include <Windows.h>

namespace rex
{
	void open_file(rsl::string_view filepath)
	{
		ShellExecuteA(NULL, "open", filepath.data(), NULL, NULL, SW_SHOW);
	}
}