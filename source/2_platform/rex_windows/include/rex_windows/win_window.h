#pragma once

#include "rex_engine/core_window.h"
#include "rex_windows/win_types.h"
#include "rex_windows/win_window_class.h"
#include "rex_windows/win_event_handler.h"

namespace rex
{
  namespace win32
  {
    class Window : public IWindow
    {
    public:
      Window();

      bool create(HInstance hInstance, s32 cmdShow, const WindowDescription& description);

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
      bool destroy();

    private:
      WindowClass m_wnd_class;
      EventHandler m_event_handler;
      Hwnd m_hwnd;
      bool m_destroyed;
    };
  } // namespace win32
} // namespace rex