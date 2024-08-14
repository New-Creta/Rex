#include "rex_directx/system/dx_resource_heap.h"

#include "rex_directx/diagnostics/dx_call.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
  namespace gfx
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
      const D3D12_RESOURCE_ALLOCATION_INFO alloc_info = m_device->GetResourceAllocationInfo(0, 1, &desc);
      REX_ASSERT_X(can_fit_allocation(alloc_info), "Trying to allocate {} bytes which would overrun resource heap of {} bytes", alloc_info.SizeInBytes, m_memory_limit);

      wrl::ComPtr<ID3D12Resource> buffer;
      if(DX_FAILED(m_device->CreatePlacedResource(m_heap.Get(), m_used_memory, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer))))
      {
        REX_ERROR(LogResourceHeap, "Failed to create buffer");
        return nullptr;
      }

      m_used_memory += static_cast<s64>(alloc_info.SizeInBytes);
      return buffer;
    }

    wrl::ComPtr<ID3D12Resource> ResourceHeap::create_texture2d(DXGI_FORMAT format, s32 width, s32 height)
    {
      CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
      const D3D12_RESOURCE_ALLOCATION_INFO alloc_info = m_device->GetResourceAllocationInfo(0, 1, &desc);
      REX_ASSERT_X(can_fit_allocation(alloc_info), "Trying to allocate {} bytes which would overrun resource heap of {} bytes", alloc_info.SizeInBytes, m_memory_limit);

      wrl::ComPtr<ID3D12Resource> texture;
      if (DX_FAILED(m_device->CreatePlacedResource(m_heap.Get(), m_used_memory, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture))))
      {
        REX_ERROR(LogResourceHeap, "Failed to create 2D texture");
        return nullptr;
      }

      m_used_memory += static_cast<s64>(alloc_info.SizeInBytes);
      return texture;
    }

    wrl::ComPtr<ID3D12Resource> ResourceHeap::create_depth_stencil_buffer(DXGI_FORMAT format, s32 width, s32 height)
    {
      CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
      desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
      const D3D12_RESOURCE_ALLOCATION_INFO alloc_info = m_device->GetResourceAllocationInfo(0, 1, &desc);
      REX_ASSERT_X(can_fit_allocation(alloc_info), "Trying to allocate {} bytes which would overrun resource heap of {} bytes", alloc_info.SizeInBytes, m_memory_limit);

      wrl::ComPtr<ID3D12Resource> texture;
      if (DX_FAILED(m_device->CreatePlacedResource(m_heap.Get(), m_used_memory, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture))))
      {
        REX_ERROR(LogResourceHeap, "Failed to create 2D texture");
        return nullptr;
      }

      m_used_memory += static_cast<s64>(alloc_info.SizeInBytes);
      return texture;
    }

    bool ResourceHeap::can_fit_allocation(const D3D12_RESOURCE_ALLOCATION_INFO& alloc_info) const
    {
      return static_cast<s64>(m_used_memory.size_in_bytes() + alloc_info.SizeInBytes) < m_memory_limit;
    }

  } // namespace gfx
} // namespace rex