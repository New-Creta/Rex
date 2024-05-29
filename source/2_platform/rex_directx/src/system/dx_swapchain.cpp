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

      s32 buffer_count = m_swapchain_buffers.size();
      m_swapchain_buffers.clear();

      if(DX_FAILED(m_swapchain->ResizeBuffers(buffer_count, width, height, m_format, flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      store_buffers(buffer_count);
    }

    s32 DxSwapchain::buffer_count() const
    {
      return m_swapchain_buffers.size();
    }

    void DxSwapchain::present()
    {
      m_swapchain->Present(0, rsl::no_flags());
    }

    Resource2* DxSwapchain::get_buffer(s32 idx)
    {
      REX_ASSERT_X(idx < m_swapchain_buffers.size(), "Buffer index out of bounds");
      return &m_swapchain_buffers[idx];
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

        auto buffer = rsl::make_unique<DxTexture2D>(d3d_buffer);
        DescriptorHandle handle = gfx::create_rtv(buffer.get());

        m_swapchain_buffers.emplace_back(buffer, handle, width(), height(), format());
      }
    }


  } // namespace rhi
} // namespace rex