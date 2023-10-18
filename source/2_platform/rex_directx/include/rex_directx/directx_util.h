#pragma once

// NOLINTBEGIN(llvm-include-order)
// clang-format off

#include "rex_directx/wrl/wrl_types.h"

#include <d3d12.h>
#include <d3d11on12.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

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