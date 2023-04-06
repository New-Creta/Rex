

#pragma once

#include "rex_std/mutex.h"

#include "rex_engine/diagnostics/logging/internal/details/periodic_worker.h"

namespace rexlog
{
  namespace details
  {

    // stop the worker thread and join it
    REXLOG_INLINE inline PeriodicWorker::~PeriodicWorker()
    {
      if(m_worker_thread.joinable())
      {
        {
          rsl::unique_lock<rsl::mutex> const lock(m_mutex);
          m_active = false;
        }
        m_cv.notify_one();
        m_worker_thread.join();
      }
    }

  } // namespace details
} // namespace rexlog
