#include "rex_directx/system/dx_resource_heap.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/utility/d3dx12.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogResourceHeap);

    ResourceHeap::ResourceHeap(const wrl::ComPtr<ID3D12Heap>& heap, const wrl::ComPtr<ID3D12Device1>& device)
        : m_heap(heap)
        , m_device(device)
        , m_used_memory(0)
        , m_memory_limit(0)
    {
      const D3D12_HEAP_DESC heap_desc = m_heap->GetDesc();
      m_memory_limit            = static_cast<s64>(heap_desc.SizeInBytes);
    }

    wrl::ComPtr<ID3D12Resource> ResourceHeap::create_buffer(rsl::memory_size size, s32 alignment)
    {
      auto desc = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE, alignment);

      wrl::ComPtr<ID3D12Resource> buffer;
      if(DX_FAILED(m_device->CreatePlacedResource(m_heap.Get(), m_used_memory, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer))))
      {
        REX_ERROR(LogResourceHeap, "Failed to create buffer");
        return nullptr;
      }

      const D3D12_RESOURCE_ALLOCATION_INFO alloc_info = m_device->GetResourceAllocationInfo(0, 1, &desc);
      m_used_memory += static_cast<s64>(alloc_info.SizeInBytes);
      return buffer;
    }

    rsl::unique_ptr<Resource> ResourceHeap::create_depth_stencil_resource(s32 width, s32 height, s32 alignment)
    {
      D3D12_RESOURCE_DESC desc = {};
      desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      desc.Alignment           = alignment;
      desc.Width               = width;
      desc.Height              = height;
      desc.DepthOrArraySize    = 1;
      desc.MipLevels           = 1;

      // SSAO requires an SRV to the depth buffer to read from
      // the depth buffer.  Therefore, because we need to create two views to the same resource:
      //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
      //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
      // we need to create the depth buffer resource with a typeless format.
      desc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
      desc.SampleDesc.Count   = 1;
      desc.SampleDesc.Quality = 0;
      desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

      D3D12_CLEAR_VALUE optimized_clear_value    = {};
      optimized_clear_value.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
      optimized_clear_value.DepthStencil.Depth   = 1.0f;
      optimized_clear_value.DepthStencil.Stencil = 0;

      D3D12_RESOURCE_ALLOCATION_INFO alloc_info = m_device->GetResourceAllocationInfo(0, 1, &desc);
      REX_ASSERT_X(static_cast<s64>(alloc_info.SizeInBytes) < (m_memory_limit - m_used_memory), "About to allocate {} bytes, but the heap only has {} memory available", alloc_info.SizeInBytes, (m_memory_limit - m_used_memory));
      wrl::ComPtr<ID3D12Resource> depth_stencil_buffer;
      if(DX_FAILED(m_device->CreatePlacedResource(m_heap.Get(), m_used_memory, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimized_clear_value, IID_PPV_ARGS(&depth_stencil_buffer))))
      {
        REX_ERROR(LogResourceHeap, "Failed to create depth stencil buffer");
      }

      m_used_memory += static_cast<s64>(alloc_info.SizeInBytes);
      return rsl::make_unique<Resource>(depth_stencil_buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, alloc_info.SizeInBytes);
    }

  } // namespace rhi
} // namespace rex