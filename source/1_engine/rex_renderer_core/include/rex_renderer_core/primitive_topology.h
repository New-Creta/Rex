#pragma once

namespace rex
{
    namespace renderer
    {
        enum class PrimitiveTopology
        {
            NONE = 0,
            POINTLIST,
            LINELIST,
            LINESTRIP,
            TRIANGLELIST,
            TRIANGLESTRIP
        };
    }
}