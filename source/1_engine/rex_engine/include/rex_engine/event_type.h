#pragma once

namespace rex
{
  namespace event_system
  {
    enum class EventType
    {
      WindowClose,
      WindowActivate,
      WindowDeactivate,
      WindowStartWindowResize,
      WindowStopWindowResize,
      WindowMinimized,
      WindowMaximized,
      WindowRestored,

      QuitApp
    };
  } // namespace event_system
} // namespace rex