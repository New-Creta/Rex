#pragma once

#include "rex_std/memory.h"
#include "rex_engine/task_system/job.h"

namespace rex
{
  namespace task_system
  {
    // A task is the interface into the task system by a user.
    // You can only ask for the thread you're currently running under
    // to wait for the job to complete.
    // You can also ask for the result, which will wait first
    // and then return the result that's saved by the job
    template <typename ReturnType>
    class Task
    {
    public:
      Task(rsl::shared_ptr<Job>&& job)
        : m_job(rsl::move(job))
      {}

      // This is a blocking function.
      // It'll wait for the job (which is running on another thread) to finish
      void wait_for_me()
      {
        m_job->wait_for_me();
      }

      // This is a blocking function.
      // It'll wait for the job (which is running on another thread) to finish
      // and then return the result
      ReturnType& result()
      {
        wait_for_me();
        return m_job->result<ReturnType>();
      }

    private:
      rsl::shared_ptr<Job> m_job;
    };

    // A task is the interface into the task system by a user.
    // You can only ask for the thread you're currently running under
    // to wait for the job to complete.
    template <>
    class Task<void>
    {
    public:
      Task(rsl::shared_ptr<Job>&& job)
        : m_job(rsl::move(job))
      {}

      // This is a blocking function.
      // It'll wait for the job (which is running on another thread) to finish
      void wait_for_me()
      {
        m_job->wait_for_me();
      }

    private:
      rsl::shared_ptr<Job> m_job;
    };
  }
}