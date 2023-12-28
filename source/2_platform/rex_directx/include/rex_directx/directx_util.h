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

#ifdef REX_DEBUG
#   define REX_ENABLE_DEBUG_SHADER_COMPILATION
#   define REX_ENABLE_DEBUG_RESOURCE_NAMES

#   define REX_ENABLE_DX12_DEBUG_LAYER
#   define REX_ENABLE_DX12_LIVE_OBJECT_REPORT
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE
#   ifdef REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_message = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_message = false; } }
#   endif
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO
#   ifdef REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_info = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_info = false; } }
#   endif
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING
#   ifdef REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_warning = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_warning = false; } }
#   endif
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR
#   ifdef REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_error = true; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_error = false; } }
#   endif
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION
#   ifdef REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_corruption = true; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_corruption = false; } }
#   endif

#   define REX_ENABLE_DXGI_DEBUG_LAYER
#   define REX_ENABLE_DXGI_LIVE_OBJECT_REPORT
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE
#   ifdef REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_message = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dx12_severity_message = false; } }
#   endif
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO
#   ifdef REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_info = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_info = false; } }
#   endif
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING
#   ifdef REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_warning = false; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_warning = false; } }
#   endif
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR
#   ifdef REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_error = true; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_error = false; } }
#   endif
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION
#   ifdef REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_corruption = true; } }
#else
namespace rex { namespace globals { constexpr bool g_enable_dxgi_severity_corruption = false; } }
#   endif
#endif

// clang-format on
// NOLINTEND(llvm-include-order)