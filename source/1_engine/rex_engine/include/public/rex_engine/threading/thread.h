#pragma once

#include "rex_engine/threading/thread_event.h"
#include "rex_std/atomic.h"
#include "rex_std/functional.h"
#include "rex_std/thread.h"

namespace rex
{
  namespace threading
  {
    namespace internal
    {
      class Thread
      {
      public:
        Thread();

        Thread(const Thread&) = delete;
        Thread(Thread&&)      = delete;

        ~Thread();

        Thread& operator=(const Thread&) = delete;
        Thread& operator=(Thread&&) = delete;

        // Run the callable on another thread
        void run(rsl::function<int(void*)>&& callable, void* arg = nullptr);

      private:
        ThreadEvent m_event;
        rsl::atomic<bool> m_should_join;
        rsl::function<int(void*)> m_callable;
        void* m_arg;
        rsl::thread m_thread;
      };

      using thread_work_func = rsl::function<int(void*)>;

      // Function implemented by the platform
      // It wraps a thread entry so that any crash gets captured
      // and crash analysis can be performed on a worker thread
      rsl::function<void()> crash_guard_thread_entry(rsl::function<void()>&& func);
    } // namespace internal
  }   // namespace threading
} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/threading/win_thread.h"
#endif