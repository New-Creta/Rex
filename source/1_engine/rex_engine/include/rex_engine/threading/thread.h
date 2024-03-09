#pragma once

#include "rex_std/atomic.h"
#include "rex_std/functional.h"
#include "rex_std/thread.h"
#include "rex_engine/threading/thread_event.h"

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
        Thread(Thread&&) = delete;

        ~Thread();

        void run(rsl::function<int(void*)>&& callable, void* arg = nullptr);

      private:
        Event m_event;
        rsl::atomic<bool> m_should_join;
        rsl::function<int(void*)> m_callable;
        void* m_arg;
        rsl::thread m_thread;
      };

      using thread_work_func = rsl::function<int(void*)>;

      rsl::function<void()> wrap_thread_entry(rsl::function<void()>&& func);
    }
  }
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/platform/win/threading/win_thread.h"
#endif