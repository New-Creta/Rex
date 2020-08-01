#pragma once

#ifdef _WINDOWS
    #include "core/win32window.h"
#else
    #error Unsupported platform
#endif

namespace rex
{
    namespace engine
    {
#ifdef _WINDOWS
    using ApplicationWindow = win32::Window;
#else
    #error Unsupported platform
#endif
    }
}