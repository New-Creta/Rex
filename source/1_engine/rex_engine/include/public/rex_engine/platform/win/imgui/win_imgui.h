#pragma once

#include "rex_engine/platform/win/win_types.h"

namespace rex
{
  bool imgui_main_window_update(win::Hwnd hwnd, card32 msg, win::WParam wparam, win::LParam lparam);
}