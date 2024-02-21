#include "rex_engine/task_system/job.h"

namespace rex
{
  namespace task_system
  {
    Job::Job(job_callable&& callable)
        : m_callable(rsl::move(callable))
        , m_finished_event("")
    {
    }

    // Run the callable that's internally stored
    void Job::run()
    {
      m_result_buffer = m_callable();
      m_finished_event.signal();
    }

    // Wait for the callable to finish, this only makes sense
    // if the job is run on a different thread
    void Job::wait_for_me()
    {
      m_finished_event.wait_for_me();
    }
  } // namespace task_system
} // namespace rex