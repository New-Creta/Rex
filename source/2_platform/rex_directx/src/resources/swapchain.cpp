#include "rex_directx/system/swapchain.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/msaa_settings.h"

#include "rex_directx/diagnostics/directx_call.h"
#include "rex_directx/dxgi/rhi.h"

namespace rex
{
  namespace renderer
  {
    Swapchain::Swapchain(wrl::ComPtr<IDXGISwapChain> swapchain)
      : BaseResource<IDXGISwapChain>(m_swapchain)
      , m_swapchain(swapchain)
    {}
  }
}