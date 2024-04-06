#include "rex_directx/system/dx_debug_interface.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/diagnostics/logging/log_macros.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogDebugInterface);

    DebugInterface::DebugInterface()
    {
      if(DX_FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_debug_interface.GetAddressOf()))))
      {
        REX_ERROR(LogDebugInterface, "Cannot get debug interace of DXGI");
        return;
      }
    }

    DebugInterface::~DebugInterface()
    {
      // DXGI - Live Objects
      if(m_debug_interface)
      {
        if(DX_FAILED(m_debug_interface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL))))
        {
          REX_ERROR(LogDebugInterface, "Cannot ReportLiveDeviceObjects of DXGI");
          return;
        }
      }
    }
  } // namespace rhi
} // namespace rex