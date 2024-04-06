#pragma once

#include "rex_engine/app/core_window.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/types.h"
#include "rex_windows/app/win_window_class.h"
#include "rex_windows/engine/win_types.h"
#include "rex_windows/event_system/win_event_handler.h"

namespace rex
{
  namespace win
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

      void* primary_display_handle() override;

      s32 width() const override;
      s32 min_width() const override;
      s32 height() const override;
      s32 min_height() const override;

      f32 aspect() const override;

      void start_resize() override;
      void stop_resize() override;
      void minimize() override;
      void maximize() override;
      void restore() override;

      bool is_resizing() const override;
      bool is_minimized() const override;
      bool is_maximized() const override;

    private:
      bool destroy();

    private:
      WindowClass m_wnd_class;
      EventHandler m_event_handler;
      Hwnd m_hwnd;
      s32 m_min_width;
      s32 m_min_height;
      StateController<WindowState> m_window_state;
    };
  } // namespace win
} // namespace rex