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

    Swapchain::Swapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, DXGI_FORMAT format, s32 bufferCount, DescriptorHeap* rtvDescHeap, DescriptorHeap* dsvDescHeap, ResourceHeap* resourceHeap)
        : m_swapchain(swapchain)
        , m_format(format)
        , m_buffer_count(bufferCount)
        , m_rtv_desc_heap(rtvDescHeap)
        , m_dsv_desc_heap(dsvDescHeap)
        , m_resource_heap(resourceHeap)
        , m_current_swapchain_buffer_idx(0)
        , m_dsv()
    {
      for (s32 i = 0; i < m_buffer_count; ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        const DescriptorHandle rtv = m_rtv_desc_heap->create_rtv(buffer.Get());
        m_swapchain_buffers.emplace_back(buffer, D3D12_RESOURCE_STATE_COMMON, 0);
        m_swapchain_rtvs.push_back(rtv);
      }
    }

    DescriptorHandle Swapchain::backbuffer_view()
    {
      return m_swapchain_rtvs[static_cast<s32>(m_swapchain->GetCurrentBackBufferIndex())];
    }

    DescriptorHandle Swapchain::depth_stencil_view()
    {
      return m_dsv;
    }

    void Swapchain::transition_backbuffer(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state)
    {
      Resource& backbuffer = m_swapchain_buffers[static_cast<s32>(m_swapchain->GetCurrentBackBufferIndex())];
      backbuffer.transition(cmdList, state);
    }

    void Swapchain::resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags)
    {
      if(DX_FAILED(m_swapchain->ResizeBuffers(m_buffer_count, width, height, m_format, flags)))
      {
        REX_ERROR(LogSwapchain, "Failed to resize swapchain buffers");
        return;
      }

      for(s32 i = 0; i < m_buffer_count; ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        const DescriptorHandle rtv = m_rtv_desc_heap->create_rtv(buffer.Get());
        m_swapchain_buffers.emplace_back(buffer, D3D12_RESOURCE_STATE_COMMON, 0);
        m_swapchain_rtvs.push_back(rtv);
      }

      m_depth_stencil_buffer = m_resource_heap->create_depth_stencil_resource(width, height);
      set_debug_name_for(m_depth_stencil_buffer->get(), "Swapchain Depth Stencil Buffer");
      m_dsv = m_dsv_desc_heap->create_dsv(m_depth_stencil_buffer->get(), DXGI_FORMAT_D24_UNORM_S8_UINT);
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
  } // namespace rhi
} // namespace rex