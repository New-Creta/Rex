#pragma once

#include "rex_std/vector.h"

namespace rex
{
  namespace diagnostics
  {
    void load_log_levels(const rsl::vector<rsl::string_view>& args);
  } // namespace diagnostics
} // namespace rex