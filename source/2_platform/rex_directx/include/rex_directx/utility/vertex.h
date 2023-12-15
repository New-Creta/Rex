#pragma once

#include "rex_renderer_core/resources/vertex.h"

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        namespace directx
        {
            struct XMVertexPosCol
            {
              DirectX::XMFLOAT3 position;
              DirectX::XMFLOAT4 color;
            };

            struct XMVertexPosNormTex
            {
              DirectX::XMFLOAT3 position;
              DirectX::XMFLOAT3 normal;
              DirectX::XMFLOAT2 uv;
            };

            XMVertexPosCol convert_to_dx(const VertexPosCol& v);
            XMVertexPosNormTex convert_to_dx(const VertexPosNormTex& v);
        }
    }
}