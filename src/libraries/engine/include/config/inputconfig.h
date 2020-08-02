#pragma once

#ifdef _WINDOWS
#include "core/win32input.h"
#else
#error Unsupported platform
#endif

namespace rex
{
    namespace engine
    {
#ifdef _WINDOWS
        using Input = win32::Input;
#else
#error Unsupported platform
#endif
    }
}