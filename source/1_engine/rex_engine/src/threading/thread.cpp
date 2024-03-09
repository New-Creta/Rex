#include "rex_engine/threading/thread.h"

namespace rex
{
  namespace threading
  {
    namespace internal
    {
      Thread::Thread()
        : m_event()
        , m_should_join(false)
        , m_callable([](void*) { return 0; })
        , m_arg(nullptr)
        , m_thread(
          crash_guard_thread_entry([this]()
            {
              while (!m_should_join)
              {
                // If code locks here, that usually means something is still holding
                // a reference to a thread handle, blocking the thread from being brought back
                // to the thread pool
                m_event.wait_for_me();
                m_event.reset();

                // Call the callable and reset it afterwards
                m_callable(m_arg);
                m_callable = [](void*) { return 0; };
              }
            })
        )
      {}

      Thread::~Thread()
      {
        // Signal the event so any blocking events will continue
        // Enable joining flag so the thread exists
        // Finally wait for the thread to join
        m_event.signal();
        m_should_join = true;
        m_thread.join();
      }

      void Thread::run(rsl::function<int(void*)>&& callable, void* arg)
      {
        m_callable = rsl::move(callable);
        m_arg = arg;

        // Signal that the callable has been set so the thread can run
        m_event.signal();
      }

    }
  }
}