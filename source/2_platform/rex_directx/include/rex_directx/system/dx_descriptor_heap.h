#pragma once

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class DescriptorHandle
    {
    public:
      DescriptorHandle() = default;
      DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE handleGpu, D3D12_DESCRIPTOR_HEAP_TYPE type, s32 size);

      DescriptorHandle& operator++();
      DescriptorHandle operator++(int);

      DescriptorHandle& operator--();
      DescriptorHandle operator--(int);

      DescriptorHandle operator+(s32 offset) const;
      DescriptorHandle& operator+=(s32 offset);

      DescriptorHandle operator-(s32 offset) const;
      DescriptorHandle& operator-=(s32 offset);

      const CD3DX12_CPU_DESCRIPTOR_HANDLE& get() const;
      const D3D12_GPU_DESCRIPTOR_HANDLE& get_gpu() const;

    private:
      CD3DX12_CPU_DESCRIPTOR_HANDLE m_handle;
      D3D12_GPU_DESCRIPTOR_HANDLE m_handle_gpu;
      D3D12_DESCRIPTOR_HEAP_TYPE m_type;
      s32 m_size;
    };

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