#pragma once

#ifdef _WINDOWS
    #include "core/win32application.h"
#else
    #error Unsupported platform
#endif

namespace rex
{
    namespace engine
    {
#ifdef _WINDOWS
    using BaseApplication = win32::Application;
#else
    #error Unsupported platform
#endif
    }
}