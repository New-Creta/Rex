#include "rex_directx/system/dx_descriptor_handle.h"

namespace rex
{
  namespace rhi
  {
    DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE handleGpu, D3D12_DESCRIPTOR_HEAP_TYPE type, s32 size)
      : m_cpu_handle(handle)
      , m_gpu_handle(handleGpu)
      , m_type(type)
      , m_size(size)
    {
    }

    DescriptorHandle& DescriptorHandle::operator++()
    {
      m_cpu_handle.Offset(1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, m_size);
      m_gpu_handle = handle_gpu;
      return *this;
    }
    DescriptorHandle DescriptorHandle::operator++(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_cpu_handle);
      m_cpu_handle.Offset(1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, m_size);
      return DescriptorHandle(handle, handle_gpu, m_type, m_size);
    }

    DescriptorHandle& DescriptorHandle::operator--()
    {
      m_cpu_handle.Offset(-1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, -m_size);
      m_gpu_handle = handle_gpu;
      return *this;
    }
    DescriptorHandle DescriptorHandle::operator--(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_cpu_handle);
      m_cpu_handle.Offset(-1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, -m_size);
      return DescriptorHandle(handle, handle_gpu, m_type, m_size);
    }

    DescriptorHandle DescriptorHandle::operator+(s32 offset) const
    {
      DescriptorHandle handle = *this;
      handle += offset;
      return handle;
    }
    DescriptorHandle& DescriptorHandle::operator+=(s32 offset)
    {
      m_cpu_handle.Offset(offset, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, m_size * offset);
      m_gpu_handle = handle_gpu;
      return *this;
    }

    DescriptorHandle DescriptorHandle::operator-(s32 offset) const
    {
      DescriptorHandle handle = *this;
      handle -= offset;
      return handle;
    }
    DescriptorHandle& DescriptorHandle::operator-=(s32 offset)
    {
      m_cpu_handle.Offset(-offset, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.InitOffsetted(m_gpu_handle, m_size * -offset);
      m_gpu_handle = handle_gpu;
      return *this;
    }

    DescriptorHandle::operator D3D12_CPU_DESCRIPTOR_HANDLE() const
    {
      return cpu_handle();
    }
    DescriptorHandle::operator D3D12_GPU_DESCRIPTOR_HANDLE() const
    {
      return gpu_handle();
    }

    const CD3DX12_CPU_DESCRIPTOR_HANDLE& DescriptorHandle::cpu_handle() const
    {
      return m_cpu_handle;
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE& DescriptorHandle::gpu_handle() const
    {
      return m_gpu_handle;
    }
  }
}