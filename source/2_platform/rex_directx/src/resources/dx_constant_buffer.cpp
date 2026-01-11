#include "rex_directx/resources/dx_constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxConstantBuffer::DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, rsl::memory_size size)
      : ConstantBuffer(resource->GetGPUVirtualAddress(), &m_view, size)
      , DxResource(resource)
      , m_view(view)
    {}

    DxResourceView* DxConstantBuffer::view()
    {
      return &m_view;
    }

    void* DxConstantBuffer::api_object() const
    {
      return dx_object();
    }

  }
}