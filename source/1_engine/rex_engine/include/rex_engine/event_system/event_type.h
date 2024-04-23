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

      Update,
      Render,

      // In some cases, WindowClose wants to close the app,
      // however, you may want to capture this to prompt the user if they really want to close the app.
      // also, you want to be able to exit the application through a menu as well.
      // WindowClose is therefore not the same as QuitApp.
      // WindowClose is what it tells you, it tells you the user wants to close the window
      // QuitApp is what it tells you, you want to quit the application.
      // One usually results in the other but they are in fact 2 different events
      QuitApp
    };
  } // namespace event_system
} // namespace rex