

#pragma once

// periodic worker thread - periodically executes the given callback function.
//
// RAII over the owned thread:
//    creates the thread on construction.
//    stops and joins the thread on destruction (if the thread is executing a callback, wait for it to finish first).

#include "rex_std/chrono.h"
#include "rex_std/condition_variable.h"
#include "rex_std/functional.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"

namespace rexlog
{
  namespace details
  {

    class REXLOG_API periodic_worker
    {
    public:
      template <typename Rep, typename Period>
      periodic_worker(const rsl::function<void()>& callback_fun, rsl::chrono::duration<Rep, Period> interval)
      {
        m_active = (interval > rsl::chrono::duration<Rep, Period>::zero());
        if(!m_active)
        {
          return;
        }

        m_worker_thread = rsl::thread(
            [this, callback_fun, interval]()
            {
              for(;;)
              {
                rsl::unique_lock<rsl::mutex> lock(this->m_mutex);
                if(this->m_cv.wait_for(lock, interval, [this] { return !this->m_active; }))
                {
                  return; // m_active == false, so exit this thread
                }
                callback_fun();
              }
            });
      }
      periodic_worker(const periodic_worker&)            = delete;
      periodic_worker& operator=(const periodic_worker&) = delete;
      // stop the worker thread and join it
      ~periodic_worker();

    private:
      bool m_active;
      rsl::thread m_worker_thread;
      rsl::mutex m_mutex;
      rsl::condition_variable m_cv;
    };
  } // namespace details
} // namespace rexlog
