#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
  class QuitApp : public Event<QuitApp>
  {
  public:
    QuitApp(rsl::string_view reason)
      : m_reason(reason)
    {}

    rsl::string_view reason() const
    {
      return m_reason;
    }

  private:
    rsl::string_view m_reason;
  };
}