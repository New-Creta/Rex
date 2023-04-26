#pragma once

#include "rex_engine/types.h"
#include "rex_engine/string/stringid.h"

namespace rex
{
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
    {
    }

    StringID title;
    WindowViewport viewport;
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

    virtual s32 width() const  = 0;
    virtual s32 height() const = 0;

    virtual f32 aspect() const = 0;
  };
} // namespace rex