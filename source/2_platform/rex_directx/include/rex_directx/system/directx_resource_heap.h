#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class ResourceHeap
    {
    public:
      ResourceHeap(const wrl::ComPtr<ID3D12Heap>& heap, const wrl::ComPtr<ID3D12Device>& device);

      Resource create_2d_resource(s32 width, s32 height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, s32 alignment = 0);
      Resource create_depth_stencil_resource(s32 width, s32 height, s32 alignment = 0);

    private:
      wrl::ComPtr<ID3D12Heap> m_heap;
      wrl::ComPtr<ID3D12Device> m_device;
      s32 m_used_memory;
      s32 m_memory_limit;
    };
  }
}