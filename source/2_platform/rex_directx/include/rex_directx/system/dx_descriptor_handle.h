#pragma once

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
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

      operator D3D12_CPU_DESCRIPTOR_HANDLE() const;
      operator D3D12_GPU_DESCRIPTOR_HANDLE() const;

      const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpu_handle() const;
      const D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle() const;

    private:
      CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpu_handle;
      D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle;
      D3D12_DESCRIPTOR_HEAP_TYPE m_type;
      s32 m_size;
    };
  }
}