#include "rex_directx/resources/dx_constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxConstantBuffer::DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, rsl::memory_size size)
      : ConstantBuffer(size)
      , m_resource(resource)
    {
      m_views.push_back(view);
    }
    DxConstantBuffer::DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, rsl::vector<DxResourceView>&& views, rsl::memory_size size)
      : ConstantBuffer(size)
      , m_resource(resource)
      , m_views(rsl::move(views))
    {}

    ID3D12Resource* DxConstantBuffer::dx_object()
    {
      return m_resource.Get();
    }
  }
}