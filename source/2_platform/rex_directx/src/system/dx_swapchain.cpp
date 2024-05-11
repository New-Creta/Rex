#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_renderer_core/rendering/msaa_settings.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogSwapchain);

    Swapchain::Swapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, DXGI_FORMAT format, s32 bufferCount)
        : m_swapchain(swapchain)
        , m_format(format)
    {
      for (s32 i = 0; i < bufferCount; ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        m_swapchain_buffers.emplace_back(buffer);
      }
    }
    void Swapchain::resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags)
    {
      s32 buffer_count = m_swapchain_buffers.size();
      m_swapchain_buffers.clear();

      if(DX_FAILED(m_swapchain->ResizeBuffers(buffer_count, width, height, m_format, flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      for(s32 i = 0; i < buffer_count; ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        m_swapchain_buffers.emplace_back(buffer);
      }
    }

    s32 Swapchain::buffer_count() const
    {
      return m_swapchain_buffers.size();
    }

    void Swapchain::present()
    {
      m_swapchain->Present(0, rsl::no_flags());
    }

    Resource2* Swapchain::get_buffer(s32 idx)
    {
      REX_ASSERT_X(idx < m_swapchain_buffers.size(), "Buffer index out of bounds");
      return &m_swapchain_buffers[idx];
    }

    IDXGISwapChain3* Swapchain::get()
    {
      return m_swapchain.Get();
    }

  } // namespace rhi
} // namespace rex