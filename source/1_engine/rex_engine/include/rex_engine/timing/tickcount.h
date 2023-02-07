#pragma once

#include "rex_std/chrono.h"

namespace rex
{
  //-------------------------------------------------------------------------
  // simulation of Windows GetTickCount()
  inline int64_t getTickCount()
  {
    return rsl::chrono::duration_cast<rsl::chrono::milliseconds>(rsl::chrono::steady_clock::now().time_since_epoch()).count();
  }

  //-------------------------------------------------------------------------
  // Clock built upon Windows GetTickCount()
  struct TickCountClock
  {
    typedef int64_t rep;
    typedef rsl::milli period;
    typedef rsl::chrono::duration<rep, period> duration;
    typedef rsl::chrono::time_point<TickCountClock> time_point;

    static const bool is_steady = true;

    //-------------------------------------------------------------------------
    static time_point now() noexcept
    {
      return time_point(duration(getTickCount()));
    }
  };
} // namespace rex