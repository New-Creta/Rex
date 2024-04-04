#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_engine/engine/types.h"

#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_resource.h"

#include <dxgi1_4.h>

namespace rex
{
  namespace rhi
  {
    struct OutputWindowUserData;
    struct MsaaSettings;

    class Swapchain
    {
    public:
      Swapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, DXGI_FORMAT format, s32 bufferCount, DescriptorHeap* rtvDescHeap, DescriptorHeap* dsvDescHeap, ResourceHeap* resourceHeap);

      DescriptorHandle backbuffer_view();
      DescriptorHandle depth_stencil_view();
      void transition_backbuffer(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state);
      void resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags);

      s32 buffer_count() const;

      void present();

      wrl::ComPtr<ID3D12Resource> get_buffer(s32 idx);

    private:
      wrl::ComPtr<IDXGISwapChain3> m_swapchain;
      DXGI_FORMAT m_format;
      rsl::vector<Resource> m_swapchain_buffers;   // swapchain render target buffer indices
      DescriptorHeap* m_rtv_desc_heap;
      DescriptorHeap* m_dsv_desc_heap;
      ResourceHeap* m_resource_heap;
      rsl::unique_ptr<Resource> m_depth_stencil_buffer;
      rsl::vector<DescriptorHandle> m_swapchain_rtvs;
      s32 m_current_swapchain_buffer_idx;
      DescriptorHandle m_dsv;

      s32 m_buffer_count;
    };
  }
}