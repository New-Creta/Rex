#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_renderer_core/gfx/msaa_settings.h"
#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"

#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogSwapchain);

    DxSwapchain::DxSwapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, s32 width, s32 height, DXGI_FORMAT format, s32 bufferCount)
      : Swapchain(width, height, d3d::from_dx12(format))
        , m_swapchain(swapchain)
        , m_format(format)
    {
      query_buffers(bufferCount);
    }

    void DxSwapchain::resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags)
    {
      // Update the cached width and height
      on_resize(width, height);

      // Empty the cached buffers
      s32 buffer_count = num_buffers();
      clear_buffers();

      // Resize the DirectX buffers
      if(DX_FAILED(m_swapchain->ResizeBuffers(buffer_count, width, height, m_format, flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      // Cache the new resized buffers
      query_buffers(buffer_count);
    }

    void DxSwapchain::present()
    {
      m_swapchain->Present(0, rsl::no_flags());
      
      // It's possible a windows error gets set because the driver wants to do something weird
      win::clear_win_errors();
    }

    IDXGISwapChain3* DxSwapchain::dx_object()
    {
      return m_swapchain.Get();
    }

    void DxSwapchain::query_buffers(s32 bufferCount)
    {
      for (s32 i = 0; i < bufferCount; ++i)
      {
        wrl::ComPtr<ID3D12Resource> d3d_buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&d3d_buffer));
        d3d::set_debug_name_for(d3d_buffer.Get(), rsl::format("DxSwapchain Back Buffer {}", i));
        auto render_target = rhi::create_render_target(d3d_buffer);
        store_render_target(rsl::move(render_target));
      }
    }

    s32 DxSwapchain::current_buffer_idx() const
    {
      return m_swapchain->GetCurrentBackBufferIndex();
    }

  } // namespace gfx
} // namespace rex