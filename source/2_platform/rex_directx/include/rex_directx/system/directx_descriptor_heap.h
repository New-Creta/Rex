#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class DescriptorHeap
    {
    public:
      DescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device);

      D3D12_CPU_DESCRIPTOR_HANDLE create_rtv(ID3D12Resource* resource);
      D3D12_CPU_DESCRIPTOR_HANDLE create_dsv(ID3D12Resource* resource, DXGI_FORMAT format);

    private:
      wrl::ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
      wrl::ComPtr<ID3D12Device1> m_device;
      D3D12_DESCRIPTOR_HEAP_TYPE m_desc_heap_type;
      s32 m_descriptor_size;
      s32 m_num_descriptors;
      s32 m_used_descriptors;
    };
  }
}