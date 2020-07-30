#pragma once

#ifdef _WINDOWS
    #ifdef _OPENGL
        #include "api/opengl/win32glcontext.h"
    #else
        #error Unsupported api
#endif
#else
    #error Unsupported platform
#endif

namespace rex
{
    namespace engine
    {
#ifdef _WINDOWS
    #ifdef _OPENGL
        using ApplicationContext = win32::Context;
    #else
        #error Unsupported api
    #endif
#else
#error Unsupported platform
#endif
    }
}