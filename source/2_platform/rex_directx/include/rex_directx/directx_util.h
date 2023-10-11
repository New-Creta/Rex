#pragma once

// NOLINTBEGIN(llvm-include-order)
// clang-format off

#include "rex_directx/wrl/wrl_types.h"

#include <d3d12.h>
#include <d3d11on12.h>

namespace rex
{
    namespace renderer
    {
        namespace directx
        {
            wrl::com_ptr<ID3D12Resource> create_default_buffer(
                ID3D12Device* device,
                ID3D12GraphicsCommandList* cmdList,
                const void* initData,
                UINT64 byteSize,
                wrl::com_ptr<ID3D12Resource>& uploadBuffer);
        }
    }
}

// clang-format on
// NOLINTEND(llvm-include-order)