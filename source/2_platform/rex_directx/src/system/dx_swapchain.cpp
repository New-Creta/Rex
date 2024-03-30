#include "rex_directx/system/dx_swapchain.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/msaa_settings.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    Swapchain::Swapchain(const wrl::ComPtr<IDXGISwapChain>& swapchain, DXGI_FORMAT format, s32 bufferCount)
      : m_swapchain(swapchain)
      , m_format(format)
      , m_buffer_count(bufferCount)
    {}

    HRESULT Swapchain::resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags)
    {
      return m_swapchain->ResizeBuffers(m_buffer_count, width, height, m_format, flags);
    }

    s32 Swapchain::buffer_count() const
    {
      return m_buffer_count;
    }

    void Swapchain::present()
    {
      m_swapchain->Present(0, rsl::no_flags());
    }

    wrl::ComPtr<ID3D12Resource> Swapchain::get_buffer(s32 idx)
    {
      REX_ASSERT_X(idx < m_buffer_count, "Buffer index out of bounds");

      wrl::ComPtr<ID3D12Resource> buffer;
      m_swapchain->GetBuffer(idx, IID_PPV_ARGS(&buffer));
      return buffer;
    }
  }
}