#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
  // pretty self explanatory what this event is for
  // a reason can be provided and an exit code
  // which will be used to exit the app with
  class QuitApp : public Event<QuitApp>
  {
  public:
    explicit QuitApp(rsl::string_view reason, s32 exitCode = 0)
      : m_reason(reason)
      , m_exit_code(exitCode)
    {}

    rsl::string_view reason() const
    {
      return m_reason;
    }
    s32 exit_code() const
    {
      return m_exit_code;
    }

  private:
    rsl::string_view m_reason;
    s32 m_exit_code;
  };
}