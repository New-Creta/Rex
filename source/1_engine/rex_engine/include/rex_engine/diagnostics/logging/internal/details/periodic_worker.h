

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
        active_ = (interval > rsl::chrono::duration<Rep, Period>::zero());
        if(!active_)
        {
          return;
        }

        worker_thread_ = rsl::thread(
            [this, callback_fun, interval]()
            {
              for(;;)
              {
                rsl::unique_lock<rsl::mutex> lock(this->mutex_);
                if(this->cv_.wait_for(lock, interval, [this] { return !this->active_; }))
                {
                  return; // active_ == false, so exit this thread
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
      bool active_;
      rsl::thread worker_thread_;
      rsl::mutex mutex_;
      rsl::condition_variable cv_;
    };
  } // namespace details
} // namespace rexlog
