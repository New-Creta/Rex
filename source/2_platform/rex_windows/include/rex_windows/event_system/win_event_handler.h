#pragma once

#include "rex_std/bonus/types.h"
#include "rex_windows/engine/win_types.h"
#include "rex_windows/imgui/gui_event_handler.h"

namespace rex
{
  class IWindow;

  namespace win
  {
    class EventHandler
    {
    public:
      explicit EventHandler(IWindow* wnd);

      LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);

    private:
      IWindow* m_wnd;
      rsl::unique_ptr<ImGUIEventHandler> m_gui_event_handler;
    };
  } // namespace win
} // namespace rex