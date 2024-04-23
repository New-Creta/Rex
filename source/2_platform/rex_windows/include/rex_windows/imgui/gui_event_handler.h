#pragma once

#include "rex_windows/engine/win_types.h"

namespace rex
{
  class ImGUIEventHandler
  {
  public:
    /**
     * Window message handler. This needs to be called by the application to allow ImGui to handle input messages.
     */
    win::LResult wnd_proc_handler(win::Hwnd hWnd, u32 msg, win::WParam wParam, win::LParam lParam);

  protected:
    friend struct rsl::default_delete<ImGUIEventHandler>;

    ImGUIEventHandler();
    virtual ~ImGUIEventHandler();
  };
} // namespace rex