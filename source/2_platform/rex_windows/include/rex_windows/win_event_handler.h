#pragma once

#include "rex_windows/win_types.h"

namespace rex 
{
  namespace win32
  {
    class EventHandler
    {
    public:
      LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);
    };
  } // namespace win32
} // namespace rex 