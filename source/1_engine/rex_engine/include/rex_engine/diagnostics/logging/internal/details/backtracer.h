

#pragma once

#include "rex_std/atomic.h"
#include "rex_std/functional.h"
#include "rex_std/mutex.h"

#include <rex_engine/diagnostics/logging/internal/details/circular_q.h>
#include <rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h>

// Store log messages in circular buffer.
// Useful for storing debug data in case of error/warning happens.

namespace rexlog
{
  namespace details
  {
    class REXLOG_API backtracer
    {
      mutable rsl::mutex mutex_;
      rsl::atomic<bool> enabled_ {false};
      circular_q<log_msg_buffer> messages_;

    public:
      backtracer() = default;
      backtracer(const backtracer& other);

      backtracer(backtracer&& other) REXLOG_NOEXCEPT;
      backtracer& operator=(backtracer other);

      void enable(size_t size);
      void disable();
      bool enabled() const;
      void push_back(const log_msg& msg);
      bool empty() const;

      // pop all items in the q and apply the given fun on each of them.
      void foreach_pop(rsl::function<void(const details::log_msg&)> fun);
    };

  } // namespace details
} // namespace rexlog