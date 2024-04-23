#include "rex_directx/resources/constant_buffer.h"

namespace rex
{
  namespace renderer
  {
    ConstantBuffer::ConstantBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource)
        : Buffer(device, resource)
    {
      m_size_in_bytes = d3d_resource_desc().Width;
    }

    ConstantBuffer::~ConstantBuffer() = default;

    size_t ConstantBuffer::size_in_bytes() const
    {
      return m_size_in_bytes;
    }
  } // namespace renderer
} // namespace rex