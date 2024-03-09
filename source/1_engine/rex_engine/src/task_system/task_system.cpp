#include "rex_engine/task_system/task_system.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/system/system_info.h"
#include "rex_engine/threading/thread_event.h"
#include "rex_engine/threading/thread_pool.h"
#include "rex_std/atomic.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/iostream.h"
#include "rex_std/mutex.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace task_system
  {
    namespace internal
    {
      class JobQueue
      {
      public:
        // check if there are any pending jobs in the queue
        bool has_pending_jobs()
        {
          const rsl::unique_lock lock(m_access_mtx);
          return !m_queued_jobs.empty();
        }

        // return the first pending job in the queue
        rsl::shared_ptr<Job> pop_job()
        {
          if(!has_pending_jobs())
          {
            return nullptr;
          }

          const rsl::unique_lock lock(m_access_mtx);
          rsl::shared_ptr<Job> job = rsl::move(m_queued_jobs.front());
          m_queued_jobs.erase(m_queued_jobs.begin());

          return job;
        }

        // Add a new job to the queue
        void push_job(rsl::shared_ptr<Job>&& job)
        {
          const rsl::unique_lock lock(m_access_mtx);
          m_queued_jobs.push_back(rsl::move(job));
        }

        // Increase the internal counter of job threads
        void inc_num_executing_job_threads()
        {
          m_num_executing_job_threads++;
        }

        // decrease the internal counter of job threads
        void dec_num_executing_job_threads()
        {
          m_num_executing_job_threads--;
        }

        // get the number of currently running job threads
        s32 num_executing_job_threads()
        {
          return m_num_executing_job_threads;
        }

      private:
        // This holds all the jobs that are queued and waiting to be executed
        // as soon as a thread becomes available
        rsl::vector<rsl::shared_ptr<Job>> m_queued_jobs;

        // A mutex that needs locking when accessing the queued jobs queue
        rsl::mutex m_access_mtx;

        // This holds the number of threads current used for running jobs
        // If this reaches 0, a new thread NEEDS to be allocated when a job gets queued
        // in order for it to get executed
        rsl::atomic<s32> m_num_executing_job_threads;
      };

      // The global job queue of all jobs that are pending execution
      JobQueue g_job_queue; // NOLINT(fuchsia-statically-constructed-objects)

      // A wrapper around a thread holding a job.
      // Jobs are owned by both the thread that executes them
      // and the task that's owned by the user
      // only when both go out of scope
      // does the job get destroyed
      class JobThread
      {
      public:
        explicit JobThread(threading::ThreadHandle thread)
            : m_thread_handle(rsl::move(thread))
            , m_job(nullptr)
            , m_wait_for_start_event()
        {
        }

        // Blocking function that waits for the job thread to have started
        void wait_for_start()
        {
          m_wait_for_start_event.wait_for_me();
        }

        // Start a job to be run on the thread that's owned by this class
        // after the job has finished, check if other jobs are pending
        // and keep using this thread to use them
        void start_job(rsl::shared_ptr<Job>&& job)
        {
          m_job = rsl::move(job);
          m_thread_handle.run(
              [this](void*)
              {
                // When this thread starts, take away the ownership of the thread handle and the job and let it be managed by this callable
                // this allows the thread that runs this callable to be automatically be added to the thread pool again after it finished executing
                const threading::ThreadHandle thread_handle = rsl::move(m_thread_handle);
                rsl::shared_ptr<Job> job                = rsl::move(m_job);

                // signal that this job thread has started
                // After this, the this point can be invalid, so we can't access it anymore
                m_wait_for_start_event.signal();

                // Keep running jobs until there are none left
                while(job)
                {
                  // Make sure we reset the job after we execute it
                  // to not leave any trailing references to it behind.
                  job->run();
                  job.reset();

                  // Check if we have any other pending jobs still in the queue
                  // If we did, pop the first and execute it on this thread
                  if(internal::g_job_queue.has_pending_jobs())
                  {
                    job = internal::g_job_queue.pop_job();
                  }
                }

                g_job_queue.dec_num_executing_job_threads();
                return 0;
              });
        }

      private:
        threading::ThreadHandle m_thread_handle;
        rsl::shared_ptr<Job> m_job;
        Event m_wait_for_start_event;
      };

      // Use the given thread and assign a job to it
      // Then start the job
      void kick_off_job(threading::ThreadHandle threadHandle, rsl::shared_ptr<Job> job)
      {
        JobThread job_thread(rsl::move(threadHandle));
        job_thread.start_job(rsl::move(job));
        job_thread.wait_for_start();
        g_job_queue.inc_num_executing_job_threads();
      }
    } // namespace internal

    // If there's a idle thread available, execute the job immediately
    // If not, assign the thread to the queue and it'll be executed
    // when all previous jobs are processed
    rsl::shared_ptr<Job> add_job_to_queue(rsl::shared_ptr<Job> job)
    {
      if(threading::has_any_idle_thread())
      {
        threading::ThreadHandle thread_handle = threading::acquire_idle_thread();
        internal::kick_off_job(rsl::move(thread_handle), rsl::move(job));
      }
      else
      {
        REX_ASSERT_X(internal::g_job_queue.num_executing_job_threads() > 0, "Didn't get any free threads from the thread pool and don't have any threads currently processing jobs");
        internal::g_job_queue.push_job(rsl::move(job));
      }

      return job;
    }
  } // namespace task_system
} // namespace rex