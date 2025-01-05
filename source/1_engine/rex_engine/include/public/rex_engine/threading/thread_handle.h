#pragma once

#include "rex_engine/threading/thread.h"

namespace rex
{
  namespace threading
  {
    // A handle that wraps a thread
    // Users are not meant to have direct access to a thread
    // Instead we return handles to threads that return the thread to the thread pool
    class ThreadHandle
    {
    public:
      explicit ThreadHandle(internal::Thread* thread);
      ThreadHandle(const ThreadHandle&) = delete;
      ThreadHandle(ThreadHandle&& other);
      ~ThreadHandle();

      ThreadHandle& operator=(const ThreadHandle&) = delete;
      ThreadHandle& operator=(ThreadHandle&& other);

      void run(internal::thread_work_func&& func, void* arg = nullptr);

      const internal::Thread* thread() const;

    private:
      void return_me_to_thread_pool();

    private:
      internal::Thread* m_thread;
    };
  } // namespace threading
} // namespace rex