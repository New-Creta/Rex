#pragma once

#include "rex_engine/task_system/job.h"
#include "rex_engine/task_system/task.h"
#include "rex_engine/threading/thread_event.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_std/type_traits.h"

namespace rex
{
  namespace task_system
  {
    namespace internal
    {
      // Create a callable that returns the result in an unique array
      // this gets filled in automatically, unless the return type is void
      // then nothing gets filled in
      template <typename ReturnType, typename Callable>
      auto create_callable(Callable&& callable)
      {
        return [moved_callable = rsl::move(callable)]()
        {
          rsl::unique_array<char> res_buffer;

          if constexpr(!rsl::is_same_v<void, ReturnType>)
          {
            res_buffer = rsl::make_unique<char[]>(sizeof(ReturnType)); // NOLINT(modernize-avoid-c-arrays)
            auto res   = moved_callable();
            memcpy(res_buffer.get(), rsl::addressof(res), sizeof(res));
          }
          else
          {
            moved_callable();
          }

          return res_buffer;
        };
      }
    } // namespace internal

    // If there's a idle thread available, execute the job immediately
    // If not, assign the thread to the queue and it'll be executed
    // when all previous jobs are processed
    rsl::shared_ptr<Job> add_job_to_queue(rsl::shared_ptr<Job> job);
  } // namespace task_system

  // Create a job to run async and wrap it in a task that's returned to the user
  // Both the thread the job runs (or the queue if there isn't an idle thread available)
  // as well as the task own the job to be run.
  // When both go out of scope, the job is destroyed.
  template <typename Callable>
  auto run_async(Callable callable)
  {
    // the callable needs to get wrapped as the thread pool only supports 1 interface callable
    // So we wrap the user's callable and call the user's callable from the wrapped one
    using return_type = rsl::invoke_result_t<Callable>;
    auto job_callable = task_system::internal::create_callable<return_type>(rsl::move(callable));

    // Create a new job which wraps the callable
    rsl::shared_ptr<task_system::Job> new_job = rsl::make_shared<task_system::Job>(rsl::move(job_callable));

    // Add the job to the queue to be scheduled for execution
    task_system::add_job_to_queue(new_job);

    // Return the task to the user so they can wait for the task's completion if needed
    const task_system::Task<return_type> task(rsl::move(new_job));
    return task;
  }
} // namespace rex