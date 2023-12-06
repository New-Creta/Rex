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
#pragma comment(lib, "dxguid.lib")

namespace rex
{
    namespace renderer
    {
        namespace directx
        {

        }
    }
}

// clang-format on
// NOLINTEND(llvm-include-order)