#pragma once

#include "rex_engine/defines.h" // for BIT
#include "rex_engine/types.h"   // for s32, f32
#include "rex_std/string.h"     // for string_view
#include "rex_std/string_view.h"

namespace rex
{
  enum class WindowState
  {
    Idle      = 0,
    Resizing  = BIT(0),
    Minimized = BIT(1),
    Maximized = BIT(2),
    Destroyed = BIT(3)
  };

  struct WindowViewport
  {
    s32 x;
    s32 y;
    s32 width;
    s32 height;
  };

  struct WindowDescription
  {
    WindowDescription()
        : title("rex")
        , viewport({0, 0, 1280, 720})
        , min_width(200)
        , min_height(200)
    {
    }

    rsl::string_view title;
    WindowViewport viewport;

    s32 min_width;
    s32 min_height;
  };

  class IWindow
  {
  public:
    IWindow()               = default;
    IWindow(const IWindow&) = delete;
    IWindow(IWindow&&)      = delete;
    virtual ~IWindow();

    IWindow& operator=(const IWindow&) = delete;
    IWindow& operator=(IWindow&&)      = delete;

    virtual void update() = 0;
    virtual void show()   = 0;
    virtual void hide()   = 0;
    virtual void close()  = 0;

    virtual void* primary_display_handle() = 0;

    virtual s32 width() const      = 0;
    virtual s32 min_width() const  = 0;
    virtual s32 height() const     = 0;
    virtual s32 min_height() const = 0;

    virtual f32 aspect() const = 0;

    virtual void start_resize() = 0;
    virtual void stop_resize()  = 0;
    virtual void minimize()     = 0;
    virtual void maximize()     = 0;
    virtual void restore()      = 0;

    virtual bool is_resizing() const  = 0;
    virtual bool is_minimized() const = 0;
    virtual bool is_maximized() const = 0;
  };
} // namespace rex