#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class ResourceHeap
    {
    public:
      ResourceHeap(const wrl::ComPtr<ID3D12Heap>& heap, const wrl::ComPtr<ID3D12Device1>& device);

      wrl::ComPtr<ID3D12Resource> create_buffer(rsl::memory_size size, s32 alignment = 0);
      rsl::unique_ptr<Resource> create_depth_stencil_resource(s32 width, s32 height, s32 alignment = 0);

    private:
      wrl::ComPtr<ID3D12Heap> m_heap;
      wrl::ComPtr<ID3D12Device> m_device;
      rsl::memory_size m_used_memory;
      rsl::memory_size m_memory_limit;
    };
  }
}