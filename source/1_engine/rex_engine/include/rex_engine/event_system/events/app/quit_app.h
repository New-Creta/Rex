#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
  class QuitApp : public Event<QuitApp>
  {
  public:
    QuitApp(rsl::string_view reason, s32 exitCode = 0)
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