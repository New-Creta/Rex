

#pragma once

#include "rex_std/mutex.h"

#include <rex_engine/diagnostics/logging/internal/details/periodic_worker.h>

namespace rexlog
{
  namespace details
  {

    // stop the worker thread and join it
    REXLOG_INLINE periodic_worker::~periodic_worker()
    {
      if(worker_thread_.joinable())
      {
        {
          rsl::unique_lock<rsl::mutex> lock(mutex_);
          active_ = false;
        }
        cv_.notify_one();
        worker_thread_.join();
      }
    }

  } // namespace details
} // namespace rexlog