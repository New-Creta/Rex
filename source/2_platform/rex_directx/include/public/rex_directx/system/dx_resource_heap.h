#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

#include "rex_engine/gfx/resources/clear_state.h"

struct D3D12_RESOURCE_ALLOCATION_INFO;

namespace rex
{
  namespace gfx
  {
    class ResourceHeap
    {
    public:
      ResourceHeap(const wrl::ComPtr<ID3D12Heap>& heap, const wrl::ComPtr<ID3D12Device1>& device);

      // Create a new 1D buffer on the gpu
      wrl::ComPtr<ID3D12Resource> create_buffer(rsl::memory_size size, s32 alignment = 0);
      wrl::ComPtr<ID3D12Resource> create_unordered_access_buffer(rsl::memory_size size, s32 alignment = 0);
      // Create a new 2D texture on the gpu
      wrl::ComPtr<ID3D12Resource> create_texture2d(DXGI_FORMAT format, s32 width, s32 height);
      wrl::ComPtr<ID3D12Resource> create_depth_stencil_buffer(DXGI_FORMAT format, s32 width, s32 height, const ClearStateDesc& clearStateDesc);

    private:
      // Verify if the heap has enough space for a new allocation
      bool can_fit_allocation(const D3D12_RESOURCE_ALLOCATION_INFO& alloc_info) const;

      // Create a 1D buffer on the gpu with the specified flags
      wrl::ComPtr<ID3D12Resource> create_buffer(rsl::memory_size size, s32 alignment, D3D12_RESOURCE_FLAGS flags);

    private:
      wrl::ComPtr<ID3D12Heap> m_heap;
      wrl::ComPtr<ID3D12Device> m_device;
      rsl::memory_size m_used_memory;
      rsl::memory_size m_memory_limit;
    };
  } // namespace gfx
} // namespace rex