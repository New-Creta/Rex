#pragma once

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        namespace math_helper
        {
            inline DirectX::XMFLOAT4X4 Identity4x4()
            {
                static DirectX::XMFLOAT4X4 I(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

                return I;
            }
        } // namespace math_helper
    }
}