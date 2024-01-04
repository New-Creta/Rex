#pragma once

#include "rex_engine/types.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{
  struct EngineParams
  {
    using init_func     = rsl::function<bool()>;
    using update_func   = rsl::function<void()>;
    using draw_func     = rsl::function<void()>;
    using shutdown_func = rsl::function<void()>;

    // how much memory is the entire app allowed to use.
    // by default, there's no limit on this.
    rsl::memory_size max_memory     = rsl::memory_size((rsl::numeric_limits<s64>::max)());
    init_func app_init_func         = nullptr;
    update_func app_update_func     = nullptr;
    draw_func app_draw_func         = nullptr;
    shutdown_func app_shutdown_func = nullptr;
  };
} // namespace rex