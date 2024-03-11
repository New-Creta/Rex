#include "rex_engine/threading/thread_handle.h"
#include "rex_engine/threading/thread_pool.h"

namespace rex
{
  namespace threading
  {
    ThreadHandle::ThreadHandle(internal::Thread* thread)
      : m_thread(thread)
    {
    }

    ThreadHandle::ThreadHandle(ThreadHandle&& other)
      : m_thread(rsl::exchange(other.m_thread, nullptr))
    {
    }

    ThreadHandle::~ThreadHandle()
    {
      return_me_to_thread_pool();
    }

    ThreadHandle& ThreadHandle::operator=(ThreadHandle&& other)
    {
      return_me_to_thread_pool();
      m_thread = rsl::exchange(other.m_thread, nullptr);

      return *this;
    }

    void ThreadHandle::run(internal::thread_work_func&& func, void* arg)
    {
      m_thread->run(rsl::move(func), arg);
    }

    void ThreadHandle::return_me_to_thread_pool()
    {
      if (m_thread)
      {
        threading::internal::global_thread_pool().return_thread(m_thread);
      }
    }
  }
}