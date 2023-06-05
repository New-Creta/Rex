#pragma once

#include "rex_std/string_view.h"

//-------------------------------------------------------------------------
// File path separator
namespace rex
{
#ifdef REX_PLATFORM_WINDOWS
  inline constexpr rsl::string_view g_folder_seps          = "\\/";
  inline constexpr rsl::string_view g_folder_seps_filename = "\\/";
#else
  inline constexpr rsl::string_view g_folder_seps          = "/";
  inline constexpr rsl::string_view g_folder_seps_filename = "/";
#endif
} // namespace rex