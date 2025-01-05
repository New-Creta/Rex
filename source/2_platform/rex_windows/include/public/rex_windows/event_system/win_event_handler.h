#pragma once

#include "rex_std/bonus/types.h"
#include "rex_engine/platform/win/win_types.h"

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
    };
  } // namespace win
} // namespace rex