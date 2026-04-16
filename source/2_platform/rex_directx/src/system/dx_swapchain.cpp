#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_gal.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/gfx/core/msaa_settings.h"
#include "rex_engine/gfx/core/renderer_output_window_user_data.h"

#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_engine/gfx/graphics.h"

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

    void DxSwapchain::present()
    {
      m_swapchain->Present(1, rsl::no_flags());
      
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
        DXGI_FORMAT format = m_format == DXGI_FORMAT_R8G8B8A8_UNORM ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : m_format;
        auto render_target = static_cast<DirectXInterface*>(gfx::gal::instance())->create_render_target(d3d_buffer, format);
        store_render_target(rsl::move(render_target));
      }
    }

    void DxSwapchain::retarget_buffers(const scratch_vector<DxResourceView>& rtvs)
    {
      for (s32 i = 0; i < rtvs.size(); ++i)
      {
        wrl::ComPtr<ID3D12Resource> d3d_buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&d3d_buffer));
        d3d::set_debug_name_for(d3d_buffer.Get(), rsl::format("DxSwapchain Back Buffer {}", i));
        DXGI_FORMAT format = m_format == DXGI_FORMAT_R8G8B8A8_UNORM ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : m_format;
        auto render_target = static_cast<DirectXInterface*>(gfx::gal::instance())->retarget_render_target(d3d_buffer, format, rtvs[i]);
        store_render_target(rsl::move(render_target));
      }
    }

    s32 DxSwapchain::current_buffer_idx() const
    {
      return m_swapchain->GetCurrentBackBufferIndex();
    }

    void DxSwapchain::resize(s32 newWidth, s32 newHeight)
    {
      // Update the cached width and height
      on_resize(newWidth, newHeight);

      // First get the views of the render targets
      s32 buffer_count = num_buffers();
      scratch_vector<DxResourceView> rtvs;
      rtvs.reserve(buffer_count);
      for (s32 i = 0; i < buffer_count; ++i)
      {
        const DxResourceView* dx_rtv = static_cast<const DxResourceView*>(buffer_at(i)->view());
        rtvs.push_back(*dx_rtv);
      }

      // Empty the cached buffers
      clear_buffers();

      DXGI_SWAP_CHAIN_DESC desc;
      DX_CALL(m_swapchain->GetDesc(&desc));
      
      // Resize the DirectX buffers
      if (DX_FAILED(m_swapchain->ResizeBuffers(buffer_count, newWidth, newHeight, m_format, desc.Flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      // Cache the new resized buffers
      retarget_buffers(rtvs);
    }

  } // namespace gfx
} // namespace rex