#include "rex_directx/system/dx_debug_interface.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/diagnostics/logging/log_macros.h"

#include "rex_directx/dxgi/includes.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogDebugInterface);

#ifdef REX_ENABLE_GFX_DEBUGGING
    constexpr bool g_enable_dx12_severity_message     = false;
    constexpr bool g_enable_dx12_severity_info        = false;
    constexpr bool g_enable_dx12_severity_warning     = false;
    constexpr bool g_enable_dx12_severity_error       = true;
    constexpr bool g_enable_dx12_severity_corruption  = true;
    constexpr bool g_enable_dxgi_severity_message     = false;
    constexpr bool g_enable_dxgi_severity_info        = false;
    constexpr bool g_enable_dxgi_severity_warning     = false;
    constexpr bool g_enable_dxgi_severity_error       = true;
    constexpr bool g_enable_dxgi_severity_corruption  = true;
#else
    constexpr bool g_enable_dx12_severity_message     = false;
    constexpr bool g_enable_dx12_severity_info        = false;
    constexpr bool g_enable_dx12_severity_warning     = false;
    constexpr bool g_enable_dx12_severity_error       = false;
    constexpr bool g_enable_dx12_severity_corruption  = false;
    constexpr bool g_enable_dxgi_severity_message     = false;
    constexpr bool g_enable_dxgi_severity_info        = false;
    constexpr bool g_enable_dxgi_severity_warning     = false;
    constexpr bool g_enable_dxgi_severity_error       = false;
    constexpr bool g_enable_dxgi_severity_corruption  = false;
#endif

    DebugInterface::DebugInterface()
    {
      init_dxgi_debug_interface();
      init_debug_controller();
      init_debug_info_queue();
    }

    DebugInterface::~DebugInterface()
    {
      // DXGI - Live Objects
      if(m_debug_interface)
      {
        if(DX_FAILED(m_debug_interface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)))) // NOLINT(hicpp-signed-bitwise)
        {
          REX_ERROR(LogDebugInterface, "Cannot ReportLiveDeviceObjects of DXGI");
          return;
        }
      }
    }

    void DebugInterface::init_dxgi_debug_interface()
    {
      if (DX_FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_debug_interface.GetAddressOf()))))
      {
        REX_ERROR(LogDebugInterface, "Cannot get debug interace of DXGI");
      }
    }
    void DebugInterface::init_debug_controller()
    {
      if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug_controller))))
      {
        m_debug_controller->EnableDebugLayer();
      }
      else
      {
        REX_ERROR(LogDebugInterface, "Cannot enable D3D12 Debug layer");
      }
    }
    void DebugInterface::init_debug_info_queue()
    {
      /*
        * Bug in the DXGI Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
        * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
        * The bug itself will be fixed in a future Windows update.
        *
        * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
        * (i.e. laptops with both Intel Integrated and discrete GPUs)
        *
        * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
        * https://github.com/walbourn/directx-vs-templates/commit/18e3eaa444e98ba75d37d506ab18df8db0b82441
      */

      if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_debug_info_queue.GetAddressOf()))))
      {
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE,     g_enable_dxgi_severity_message);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO,        g_enable_dxgi_severity_info);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING,     g_enable_dxgi_severity_warning);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,       g_enable_dxgi_severity_error);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,  g_enable_dxgi_severity_corruption);

        rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide =
        {
            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
        };

        DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
        dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
        dxgi_filter.DenyList.pIDList = dxgi_hide.data();

        m_debug_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);
      }
      else
      {
        REX_ERROR(LogDebugInterface, "Unable to get DXGI Debug Info Queue");
      }
    }


  } // namespace rhi
} // namespace rex