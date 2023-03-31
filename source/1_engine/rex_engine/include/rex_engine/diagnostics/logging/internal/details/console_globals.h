

#pragma once

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>

namespace rexlog
{
  namespace details
  {

    struct console_mutex
    {
      using mutex_t = rsl::mutex;
      static mutex_t& mutex()
      {
        static mutex_t s_mutex;
        return s_mutex;
      }
    };

    struct console_nullmutex
    {
      using mutex_t = null_mutex;
      static mutex_t& mutex()
      {
        static mutex_t s_mutex;
        return s_mutex;
      }
    };
  } // namespace details
} // namespace rexlog
