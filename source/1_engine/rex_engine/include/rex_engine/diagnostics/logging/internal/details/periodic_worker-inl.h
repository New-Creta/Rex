

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/periodic_worker.h"
#include "rex_std/mutex.h"

namespace rexlog
{
  namespace details
  {

    // stop the worker thread and join it
    REXLOG_INLINE PeriodicWorker::~PeriodicWorker()
    {
      if(m_worker_thread.joinable())
      {
        {
          const rsl::unique_lock<rsl::mutex> lock(m_mutex);
          m_active = false;
        }
        m_cv.notify_one();
        m_worker_thread.join();
      }
    }

  } // namespace details
} // namespace rexlog
