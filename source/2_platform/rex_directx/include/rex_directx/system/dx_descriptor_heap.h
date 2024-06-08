#pragma once

#include "rex_directx/system/dx_descriptor_handle.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class DescriptorHeap
    {
    public:
      DescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device);

      DescriptorHandle create_rtv(ID3D12Resource* resource);
      DescriptorHandle create_dsv(ID3D12Resource* resource, DXGI_FORMAT format);
      DescriptorHandle create_cbv(ID3D12Resource* resource, rsl::memory_size size);
      DescriptorHandle create_texture2d_srv(ID3D12Resource* resource);

      D3D12_GPU_DESCRIPTOR_HANDLE gpu_heap_start();

      ID3D12DescriptorHeap* get();
      DescriptorHandle new_free_handle();

      void reset();

    private:
      DescriptorHandle my_start_handle();

    private:
      wrl::ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
      wrl::ComPtr<ID3D12Device1> m_device;
      D3D12_DESCRIPTOR_HEAP_TYPE m_desc_heap_type;
      s32 m_descriptor_size;
      s32 m_num_descriptors;
      s32 m_used_descriptors;
    };
  } // namespace rhi
} // namespace rex