#pragma once

#ifdef _WINDOWS
#include "core/win32input.h"

#include "events/input/win32keydown.h"
#include "events/input/win32keyup.h"
#include "events/input/win32keytyped.h"

#else
#error Unsupported platform
#endif

namespace rex
{
    namespace engine
    {
#ifdef _WINDOWS
        using Input = win32::Input;

        using KeyDown = win32::KeyDown;
        using KeyUp = win32::KeyUp;
        using KeyTyped = win32::KeyTyped;
#else
#error Unsupported platform
#endif
    }
}