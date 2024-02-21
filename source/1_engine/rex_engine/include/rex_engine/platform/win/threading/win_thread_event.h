#pragma once

#include "rex_std/bonus/platform.h"

#include "rex_std/string_view.h"

namespace rex
{
  // A wrapper around a OS's thread event.
  // A thread event can be used to signal a state on one thread
  // and wait for the that signal on another thread
  // Note: Waiting for a signal is blocking.
  class Event
  {
  public:
    Event(rsl::string_view name = "");

    // Destroy this event
    // Useful in case it's held by a thread and needs to get manually destroyed
    void destroy();

    // Signal the event. This signal is what another thread could be waiting for
    void signal();

    // Reset the event to a non-signaled state
    void reset();

    // Wait for the event to be signaled
    // Note: this is blocking
    void wait_for_me();

  private:
    rsl::win::handle m_handle;
  };
}