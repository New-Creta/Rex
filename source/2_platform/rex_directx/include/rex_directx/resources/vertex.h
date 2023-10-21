#pragma once

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        namespace directx
        {
            struct VertexPosCol
            {
              DirectX::XMFLOAT3 position;
              DirectX::XMFLOAT4 color;
            };

            struct VertexPosNormTex
            {
              DirectX::XMFLOAT3 position;
              DirectX::XMFLOAT3 normal;
              DirectX::XMFLOAT2 uv;
            };
        }
    }
}