#pragma once

#include "rex_std/functional.h"

namespace rex
{
  namespace thread_pool
  {
    namespace internal
    {
      class Thread;
    }

    // A handle that wraps a thread
    // Users are not meant to have direct access to a thread
    // Instead we return handles to threads that return the thread to the thread pool
    class ThreadHandle
    {
    public:
      using thread_work_func = rsl::function<int(void*)>;

      ThreadHandle(internal::Thread* thread);
      ThreadHandle(const ThreadHandle&) = delete;
      ThreadHandle(ThreadHandle&&);
      ~ThreadHandle();

      ThreadHandle& operator=(const ThreadHandle&) = delete;
      ThreadHandle& operator=(ThreadHandle&&);

      void run(thread_work_func&& func, void* arg = nullptr);

    private:
      void return_me_to_thread_pool();

    private:
      internal::Thread* m_thread;
    };

    // This checks if we have any threads in the thread pool that are idle and ready for use
    bool has_any_idle_thread();

    // This is a blocking func as there isn't always an idle thread available
    ThreadHandle acquire_idle_thread();
  }
}