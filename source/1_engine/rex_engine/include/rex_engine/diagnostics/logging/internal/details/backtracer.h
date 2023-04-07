

#pragma once

#include "rex_std/atomic.h"
#include "rex_std/functional.h"
#include "rex_std/mutex.h"

#include "rex_engine/diagnostics/logging/internal/details/circular_q.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"

// Store log messages in circular buffer.
// Useful for storing debug data in case of error/warning happens.

namespace rexlog
{
  namespace details
  {
    class REXLOG_API Backtracer
    {
      mutable rsl::mutex m_mutex;
      rsl::atomic<bool> m_enabled {false};
      CircularQ<LogMsgBuffer> m_messages;

    public:
      Backtracer() = default;
      Backtracer(const Backtracer& other);
      Backtracer(Backtracer&& other) REXLOG_NOEXCEPT;
      ~Backtracer();
      Backtracer& operator=(const Backtracer& other);
      Backtracer& operator=(Backtracer&& other);

      void enable(size_t size);
      void disable();
      bool enabled() const;
      void push_back(const LogMsg& msg);
      bool empty() const;

      // pop all items in the q and apply the given fun on each of them.
      void foreach_pop(const rsl::function<void(const details::LogMsg&)>& fun);
    };

  } // namespace details
} // namespace rexlog