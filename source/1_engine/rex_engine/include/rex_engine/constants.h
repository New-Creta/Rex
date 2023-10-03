#pragma once

#include "rex_std/string_view.h"

//-------------------------------------------------------------------------
// End of line
namespace rex
{
#ifdef REX_PLATFORM_WINDOWS
  inline constexpr rsl::string_view g_default_eol = "\r\n";
#else
  inline constexpr rsl::string_view g_default_eol = "\n";
#endif
} // namespace rex