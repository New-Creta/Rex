#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/optional.h"

// Some forward declarations of Windows internal structs
struct HKEY__;
typedef HKEY__* HKEY;

namespace rex
{
	namespace win
	{
		namespace registry
		{
			rsl::optional<rsl::string> query_key(HKEY hkey, rsl::string_view subKey, rsl::string_view valueName);
		}
	}
}