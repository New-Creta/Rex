#pragma once

#include "rex_std/functional.h"

#include "rex_engine/threading/thread.h"
#include "rex_engine/threading/thread_handle.h"

#include "rex_std/vector.h"
#include "rex_std/mutex.h"

namespace rex
{
  namespace threading
  {
    namespace internal
    {
      class ThreadPool
      {
      public:
        ThreadPool();

        // Query if we have any idle threads available
        // An idle thread is a thread that's not executing a job at the moment.
        bool has_idle_threads();

        // Check if we have an idle thread
        // if so return it.
        Thread* acquire_idle_thread();

        // Return a thread back to the pool
        void return_thread(Thread* thread);

        // Destroy all threads. Used at shutdown of the engine
        void destroy_threads();

      private:
        rsl::vector<rsl::unique_ptr<Thread>> m_threads; // Holds and owns all the threads used by the thread pool
        rsl::vector<Thread*> m_idle_threads;            // Holds but doesn't own all the idle threads
        rsl::mutex m_threads_access_mtx;                // Mutex that's used to access the thread pool
      };
    }

    // This checks if we have any threads in the thread pool that are idle and ready for use
    bool has_any_idle_thread();

    // This is a blocking func as there isn't always an idle thread available
    ThreadHandle acquire_idle_thread();
  } // namespace threading
} // namespace rex
