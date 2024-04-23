#include "rex_engine/threading/thread_event.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rex
{
  Event::Event(rsl::string_view name)
  {
    // create an auto-reset event object ad let the system automatically reset
    // the event state to non signaled after a single waiting thread has been release
    const bool manual_reset = false;

    // Start the event non signaled so the user needs to signal it themselves
    const bool start_signaled = false;

    m_handle = rsl::win::handle(CreateEventExA(nullptr, name.data(), 0, EVENT_ALL_ACCESS));
  }

  void Event::destroy()
  {
    m_handle.close();
  }

  void Event::signal()
  {
    SetEvent(m_handle.get());
  }

  void Event::reset()
  {
    ResetEvent(m_handle.get());
  }

  void Event::wait_for_me()
  {
    WaitForSingleObject(m_handle.get(), INFINITE);
  }
} // namespace rex