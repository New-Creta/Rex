#pragma once

#ifdef _WINDOWS
#ifdef _OPENGL
#include "api/opengl/core/glshader.h"
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
        using Shader = opengl::Shader;
#else
#error Unsupported api
#endif
#else
#error Unsupported platform
#endif
    }
}