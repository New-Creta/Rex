#include "rex_engine/threading/thread_pool.h"

#include "rex_engine/system/system_info.h"
#include "rex_engine/threading/thread_event.h"
#include "rex_std/atomic.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"

namespace rex
{
  namespace thread_pool
  {
    namespace internal
    {
      // Internal classes
      // ----------------------------------
      class Thread
      {
      public:
        Thread()
            : m_event()
            , m_should_join(false)
            , m_callable([](void*) { return 0; })
            , m_arg(nullptr)
            , m_thread(
              wrap_thread_entry([this]()
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

        Thread(const Thread&) = delete;
        Thread(Thread&&)      = delete;

        ~Thread()
        {
          // Signal the event so any blocking events will continue
          // Enable joining flag so the thread exists
          // Finally wait for the thread to join
          m_event.signal();
          m_should_join = true;
          m_thread.join();
        }

        Thread& operator=(const Thread&) = delete;
        Thread& operator=(Thread&&)      = delete;

        void run(rsl::function<int(void*)>&& callable, void* arg = nullptr)
        {
          m_callable = rsl::move(callable);
          m_arg      = arg;

          // Signal that the callable has been set so the thread can run
          m_event.signal();
        }

      private:
        Event m_event;
        rsl::atomic<bool> m_should_join;
        rsl::function<int(void*)> m_callable;
        void* m_arg;
        rsl::thread m_thread;
      };

      // The thread pool class.
      // The thread pool holds all the thread that are used by the jobs
      class ThreadPool
      {
      public:
        ThreadPool()
        {
          // To maximize core usage, we query the amount of local processors
          // and create a thread for each one of them.
          const s32 num_threads_to_spawn = rex::sys_info::number_logical_processors();
          m_threads.reserve(num_threads_to_spawn);
          m_idle_threads.reserve(num_threads_to_spawn);

          for(s32 idx = 0; idx < num_threads_to_spawn; ++idx)
          {
            m_threads.push_back(rsl::make_unique<Thread>());
            m_idle_threads.push_back(m_threads.back().get());
          }
        }

        // Query if we have any idle threads available
        // An idle thread is a thread that's not executing a job at the moment.
        bool has_idle_threads()
        {
          const rsl::unique_lock lock(m_threads_access_mtx);
          return !m_idle_threads.empty();
        }

        // Check if we have an idle thread
        // if so return it.
        Thread* acquire_idle_thread()
        {
          if(has_idle_threads())
          {
            const rsl::unique_lock lock(m_threads_access_mtx);
            Thread* thread = m_idle_threads.back();
            m_idle_threads.pop_back();
            return thread;
          }

          return nullptr;
        }

        // Return a thread back to the pool
        void return_thread(Thread* thread)
        {
          const rsl::unique_lock lock(m_threads_access_mtx);
          m_idle_threads.push_back(thread);
        }

        // Destroy all threads. Used at shutdown of the engine
        void destroy_threads()
        {
          const rsl::unique_lock lock(m_threads_access_mtx);
          m_idle_threads.clear();
          m_threads.clear();
        }

      private:
        rsl::vector<rsl::unique_ptr<Thread>> m_threads; // Holds and owns all the threads used by the thread pool
        rsl::vector<Thread*> m_idle_threads;            // Holds but doesn't own all the idle threads
        rsl::mutex m_threads_access_mtx;                // Mutex that's used to access the thread pool
      };

      ThreadPool& global_thread_pool()
      {
        static ThreadPool thread_pool;
        return thread_pool;
      }
    } // namespace internal

    // Implementation
    // ----------------------------------

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
      if(m_thread)
      {
        thread_pool::internal::global_thread_pool().return_thread(m_thread);
      }
    }

    bool has_any_idle_thread()
    {
      return internal::global_thread_pool().has_idle_threads();
    }

    ThreadHandle acquire_idle_thread()
    {
      return ThreadHandle(internal::global_thread_pool().acquire_idle_thread());
    }

  } // namespace thread_pool
} // namespace rex