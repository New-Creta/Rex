#pragma once

#include "rex_engine/platform.h"
#include "rex_std/array.h"
#include "rex_std/format.h"

namespace rex
{
  namespace win
  {
    using CallStack = rsl::array<void*, 10>;

    CallStack current_callstack();
  } // namespace win
} // namespace rex
