#include "rex_directx/resources/swapchain.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/msaa_settings.h"

#include "rex_directx/diagnostics/directx_call.h"
#include "rex_directx/dxgi/rhi.h"

namespace rex
{
  namespace renderer
  {
    Swapchain::Swapchain(s32 numBackBuffers, const OutputWindowUserData& userData, DXGI_FORMAT backBufferFormat, const MsaaSettings& msaaSettings, ID3D12CommandList* commandlist)
      : BaseResource<IDXGISwapChain>(m_swapchain)
    {
      DXGI_SWAP_CHAIN_DESC sd;
      sd.BufferDesc.Width = userData.window_width;
      sd.BufferDesc.Height = userData.window_height;
      sd.BufferDesc.RefreshRate.Numerator = userData.refresh_rate;
      sd.BufferDesc.RefreshRate.Denominator = 1;
      sd.BufferDesc.Format = backBufferFormat;
      sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
      sd.SampleDesc.Count = msaaSettings.count;
      sd.SampleDesc.Quality = msaaSettings.quality;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.BufferCount = numBackBuffers;
      sd.OutputWindow = (HWND)userData.primary_display_handle;
      sd.Windowed = userData.windowed;
      sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

      m_swapchain = rhi::create_swapchain(commandlist, sd);
    }
  }
}