#include "rex_directx/resources/dx_constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxConstantBuffer::DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, rsl::memory_size size)
      : ConstantBuffer(resource->GetGPUVirtualAddress(), &m_view, size)
      , m_resource(resource)
      , m_view(view)
    {}

    ID3D12Resource* DxConstantBuffer::dx_object()
    {
      return m_resource.Get();
    }
    DxResourceView* DxConstantBuffer::view()
    {
      return &m_view;
    }

  }
}