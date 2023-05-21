

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

#include <mutex>

namespace rexlog
{
  namespace details
  {
    struct ConsoleMutex
    {
      using mutex_t = rsl::mutex;
      static mutex_t& mutex()
      {
        static mutex_t s_mutex;
        return s_mutex;
      }
    };

    struct ConsoleNullMutex
    {
      using mutex_t = NullMutex;
      static mutex_t& mutex()
      {
        static mutex_t s_mutex;
        return s_mutex;
      }
    };
  } // namespace details
} // namespace rexlog
