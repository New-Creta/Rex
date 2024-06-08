#include "rex_engine/threading/thread_event.h"
#include "rex_std/bonus/utility.h"

#include <Windows.h>

namespace rex
{
  ThreadEvent::ThreadEvent(rsl::string_view name)
  {
    // create an auto-reset event object ad let the system automatically reset
    // the event state to non signaled after a single waiting thread has been release
    const bool manual_reset = false;

    // Start the event non signaled so the user needs to signal it themselves
    const bool start_signaled = false;

    m_handle = rsl::win::handle(CreateEventExA(rsl::Nullptr<SECURITY_ATTRIBUTES>, name.data(), rsl::no_flags(), EVENT_ALL_ACCESS));
  }

  void ThreadEvent::destroy()
  {
    m_handle.close();
  }

  void ThreadEvent::signal()
  {
    SetEvent(m_handle.get());
  }

  void ThreadEvent::reset()
  {
    ResetEvent(m_handle.get());
  }

  void ThreadEvent::wait_for_me()
  {
    WaitForSingleObject(m_handle.get(), INFINITE);
  }

  rsl::win::handle_t ThreadEvent::get() const
  {
    return m_handle.get();
  }

} // namespace rex