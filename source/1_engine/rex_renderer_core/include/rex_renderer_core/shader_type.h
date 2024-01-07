#pragma once

namespace rex
{
    namespace renderer
    {
        enum class ShaderType
        {
            NONE = 0,
            VERTEX,
            PIXEL,
            GEOMETRY,
            COMPUTE
        };
    }
}