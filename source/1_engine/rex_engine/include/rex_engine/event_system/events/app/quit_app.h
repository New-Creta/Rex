#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
  // This doesn't hold any data, just that the app will quit
  class QuitApp : public Event<QuitApp>
  {
  };
}