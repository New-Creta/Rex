#pragma once

#include "rex_engine/core_window.h"

namespace rex
{
  namespace linux
  {
    class Window : public IWindow
    {
    public:
      Window();

      bool create(const WindowDescription& description);

    public:
      void update() override;
      void show() override;
      void hide() override;
      void close() override;

      void* primary_display_handle() override;

      s32 width() const override;
      s32 height() const override;

      f32 aspect() const override;

    private:
      bool destroy();

    private:
      WindowViewport m_frame;
      bool m_destroyed;
    };
  } // namespace win32
} // namespace rex