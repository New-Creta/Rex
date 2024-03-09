#pragma once

#include "rex_engine/threading/thread_event.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace task_system
  {
    using job_callable = rsl::function<rsl::unique_array<rsl::byte>()>;

    // A job is a wrapper around a callable
    // It can possibly return a result which is the return type of the callable
    // A user is not expect to create a job themselves, instead they should
    // use the task system api to run code async which will create a job for the user
    class Job
    {
    public:
      explicit Job(job_callable&& callable);

      // Run the callable that's internally stored
      void run();

      // Wait for the callable to finish, this only makes sense
      // if the job is run on a different thread
      void wait_for_me();

      // Return the results of the callable
      template <typename T>
      T& result()
      {
        return *reinterpret_cast<T*>(m_result_buffer.get());
      }

    private:
      rsl::unique_array<rsl::byte> m_result_buffer;
      job_callable m_callable;
      Event m_finished_event;
    };
  } // namespace task_system
} // namespace rex