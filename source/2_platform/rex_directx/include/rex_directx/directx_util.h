#pragma once

// NOLINTBEGIN(llvm-include-order)
// clang-format off

#include "rex_engine/platform/win/win_com_ptr.h"

#include <d3d12.h>
#include <d3d11on12.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


  
    

    
  


#if defined(REX_BUILD_DEBUG) || defined(REX_BUILD_DEBUG_OPT)
#define REX_ENABLE_GFX_DEBUGGING
#endif

#ifdef REX_ENABLE_GFX_DEBUGGING
#   define REX_ENABLE_DEBUG_SHADER_COMPILATION 1
#   define REX_ENABLE_DEBUG_RESOURCE_NAMES 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION 1
#   define REX_ENABLE_DXGI_DEBUG_LAYER 1
#   define REX_ENABLE_DXGI_LIVE_OBJECT_REPORT 1
#   define REX_ENABLE_DX12_DEBUG_LAYER 1
#   define REX_ENABLE_DX12_LIVE_OBJECT_REPORT 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE 1
# else
#   define REX_ENABLE_DEBUG_SHADER_COMPILATION 0
#   define REX_ENABLE_DEBUG_RESOURCE_NAMES 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION 0
#   define REX_ENABLE_DXGI_DEBUG_LAYER 0
#   define REX_ENABLE_DXGI_LIVE_OBJECT_REPORT 0
#   define REX_ENABLE_DX12_DEBUG_LAYER 0
#   define REX_ENABLE_DX12_LIVE_OBJECT_REPORT 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE 0
#endif

namespace rex 
{ 
  namespace globals 
  { 
    constexpr bool g_enable_dx12_severity_message     = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE; 
    constexpr bool g_enable_dx12_severity_info        = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO;
    constexpr bool g_enable_dx12_severity_warning     = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING;
    constexpr bool g_enable_dx12_severity_error       = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR;
    constexpr bool g_enable_dx12_severity_corruption  = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION;
    constexpr bool g_enable_dxgi_severity_message     = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE;
    constexpr bool g_enable_dxgi_severity_info        = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO;
    constexpr bool g_enable_dxgi_severity_warning     = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING;
    constexpr bool g_enable_dxgi_severity_error       = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR;
    constexpr bool g_enable_dxgi_severity_corruption  = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION;
  } // namespace globals 
} // namespace rex

// clang-format on
// NOLINTEND(llvm-include-order)