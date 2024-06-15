#include "rex_directx/resources/dx_constant_buffer.h"

namespace rex
{
  namespace rhi
  {
    DxConstantBuffer::DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, rsl::memory_size size)
      : ConstantBuffer(size)
      , m_resource(resource)
      , m_handle(handle)
    {}

    ID3D12Resource* DxConstantBuffer::dx_object()
    {
      return m_resource.Get();
    }
  }
}