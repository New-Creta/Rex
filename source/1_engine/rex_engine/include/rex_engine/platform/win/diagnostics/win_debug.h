#pragma once

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/platform/win/win_types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  namespace win
  {
    rsl::medium_stack_string report_win_error(HResult hr, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr);
  } // namespace win
} // namespace rex