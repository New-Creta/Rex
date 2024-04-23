#pragma once

#include "rex_directx/directx_util.h"

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        struct XMVertexPosCol
        {
            explicit XMVertexPosCol(const DirectX::XMFLOAT3& inPosition, const DirectX::XMFLOAT3& inColor)
                : position(inPosition)
                , color(inColor)
            {}

            explicit XMVertexPosCol(DirectX::FXMVECTOR inPosition, DirectX::FXMVECTOR inColor)
            {
                DirectX::XMStoreFloat3(&(this->position), inPosition);
                DirectX::XMStoreFloat3(&(this->color), inColor);
            }

            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 color;

            static const D3D12_INPUT_LAYOUT_DESC    input_layout;
            static const int                        input_element_count = 2;
            static const D3D12_INPUT_ELEMENT_DESC   input_elements[input_element_count];
        };
    }
}