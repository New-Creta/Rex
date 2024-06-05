#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_renderer_core/rendering/msaa_settings.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"

#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogSwapchain);

    DxSwapchain::DxSwapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, DXGI_FORMAT format, s32 bufferCount)
        : m_swapchain(swapchain)
        , m_format(format)
    {
      store_buffers(bufferCount);
    }
    void DxSwapchain::resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags)
    {
      on_resize(width, height);
      s32 buffer_count = num_buffers();
      clear_buffers();

      if(DX_FAILED(m_swapchain->ResizeBuffers(buffer_count, width, height, m_format, flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      store_buffers(buffer_count);
    }

    void DxSwapchain::present()
    {
      m_swapchain->Present(0, rsl::no_flags());
    }

    IDXGISwapChain3* DxSwapchain::get()
    {
      return m_swapchain.Get();
    }

    void DxSwapchain::store_buffers(s32 bufferCount)
    {
      for (s32 i = 0; i < bufferCount; ++i)
      {
        wrl::ComPtr<ID3D12Resource> d3d_buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&d3d_buffer));
        set_debug_name_for(d3d_buffer.Get(), rsl::format("DxSwapchain Back Buffer {}", i));
        auto texture = rhi::create_texture2d(d3d_buffer);
        store_buffer(rsl::move(texture));
      }
    }

    s32 DxSwapchain::current_buffer_idx() const
    {
      return m_swapchain->GetCurrentBackBufferIndex();
    }

  } // namespace rhi
} // namespace rex