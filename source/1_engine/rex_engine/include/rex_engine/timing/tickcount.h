#pragma once

#include "rex_std/chrono.h"

namespace rex
{
  //-------------------------------------------------------------------------
  // simulation of Windows GetTickCount()
  inline int64_t get_tick_count()
  {
    return rsl::chrono::duration_cast<rsl::chrono::milliseconds>(rsl::chrono::steady_clock::now().time_since_epoch()).count();
  }

  //-------------------------------------------------------------------------
  // Clock built upon Windows GetTickCount()
  struct TickCountClock
  {
    using rep        = int64;
    using period     = rsl::milli;
    using duration   = rsl::chrono::duration<rep, period>;
    using time_point = rsl::chrono::time_point<TickCountClock>;

    static const bool s_is_steady = true;

    //-------------------------------------------------------------------------
    static time_point now() noexcept
    {
      return time_point(duration(get_tick_count()));
    }
  };
} // namespace rex