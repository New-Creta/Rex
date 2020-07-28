#pragma once



#include <Windows.h>
#include <memory>

#include "math/point.h"

#include "events/input/mouseevent.h"
#include "events/input/mousescrollmeta.h"

namespace rex
{
    enum class MouseCode;

    class Window;
    class Event;

    namespace win32
    {
        rex::MousePosition getMousePositionFromClient(Window* window, LPARAM param);
        rex::MousePosition getMousePositionFromScreen(Window* window, LPARAM param);
        rex::ScrollMetaData getScrollMetaData(WPARAM param);
        events::MouseKeyMetaData getMouseKeyMetaData(WPARAM param);
    }
}
