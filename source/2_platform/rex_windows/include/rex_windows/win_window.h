#pragma once

#include "rex_engine/core_window.h"

#include "rex_windows/win_types.h"
#include "rex_windows/win_window_class.h"

namespace rex
{
  namespace win32
  {
    class Window : public IWindow
    {
    public:
      Window();

      bool create(HInstance hInstance, s32 cmdShow, const WindowDescription& description);
      bool destroy();

      LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);

    public:
      void update() override;
      void show() override;
      void hide() override;
      void close() override;

      void* get_primary_display_handle() override;

      s32 width() const override;
      s32 height() const override;

      f32 get_aspect() const override;

    private:
      WindowClass m_wnd_class;
      Hwnd m_hwnd;
      bool m_destroyed;
    };
  }
}