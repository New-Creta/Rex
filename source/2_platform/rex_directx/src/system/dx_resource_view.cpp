#include "rex_directx/system/dx_resource_view.h"

namespace rex
{
  namespace gfx
  {
    DxResourceView::DxResourceView(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE handleGpu, D3D12_DESCRIPTOR_HEAP_TYPE type, s32 size)
      : m_cpu_handle(handle)
      , m_gpu_handle(handleGpu)
      , m_type(type)
      , m_size(size)
    {
    }

    DxResourceView& DxResourceView::operator++()
    {
      m_cpu_handle.Offset(1, m_size);
      m_gpu_handle.Offset(1, m_size);
      return *this;
    }
    DxResourceView DxResourceView::operator++(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_cpu_handle);
      m_cpu_handle.Offset(1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.Offset(1, m_size);
      return DxResourceView(handle, handle_gpu, m_type, m_size);
    }

    DxResourceView& DxResourceView::operator--()
    {
      m_cpu_handle.Offset(-1, m_size);
      m_gpu_handle.Offset(-1, m_size);
      return *this;
    }
    DxResourceView DxResourceView::operator--(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_cpu_handle);
      m_cpu_handle.Offset(-1, m_size);
      CD3DX12_GPU_DESCRIPTOR_HANDLE handle_gpu(m_gpu_handle);
      handle_gpu.Offset(-1, m_size);
      return DxResourceView(handle, handle_gpu, m_type, m_size);
    }

    DxResourceView DxResourceView::operator+(s32 offset) const
    {
      DxResourceView handle = *this;
      handle += offset;
      return handle;
    }
    DxResourceView& DxResourceView::operator+=(s32 offset)
    {
      m_cpu_handle.Offset(offset, m_size);
      m_gpu_handle.Offset(offset, m_size);
      return *this;
    }

    DxResourceView DxResourceView::operator-(s32 offset) const
    {
      DxResourceView handle = *this;
      handle -= offset;
      return handle;
    }
    DxResourceView& DxResourceView::operator-=(s32 offset)
    {
      m_cpu_handle.Offset(-offset, m_size);
      m_gpu_handle.Offset(-offset, m_size);
      return *this;
    }

    DxResourceView::operator D3D12_CPU_DESCRIPTOR_HANDLE() const
    {
      return cpu_handle();
    }
    DxResourceView::operator D3D12_GPU_DESCRIPTOR_HANDLE() const
    {
      return gpu_handle();
    }

    const CD3DX12_CPU_DESCRIPTOR_HANDLE& DxResourceView::cpu_handle() const
    {
      return m_cpu_handle;
    }

    const D3D12_GPU_DESCRIPTOR_HANDLE& DxResourceView::gpu_handle() const
    {
      return m_gpu_handle;
    }
  }
}