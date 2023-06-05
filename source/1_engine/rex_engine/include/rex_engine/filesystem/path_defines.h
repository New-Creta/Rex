#pragma once

#include "rex_std/string_view.h"

#ifndef REX_PATH_DEFINES
  #define REX_PATH_DEFINES

//-------------------------------------------------------------------------
// File path separator
namespace rex
{
  #ifndef REX_FOLDER_SEPS
    #ifdef REX_PLATFORM_WINDOWS
  inline constexpr rsl::string_view g_folder_seps          = "\\/";
  inline constexpr rsl::string_view g_folder_seps_filename = "\\/";
    #else
  inline constexpr rsl::string_view g_folder_seps          = "/";
  inline constexpr rsl::string_view g_folder_seps_filename = "/";
    #endif
  #endif
} // namespace rex

#endif