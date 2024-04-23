#include "rex_directx/resources/byte_address_buffer.h"

namespace rex
{
  namespace renderer
  {
    ByteAddressBuffer::ByteAddressBuffer(Device& device, const D3D12_RESOURCE_DESC& resDesc)
        : Buffer(device, resDesc)
    {
    }

    ByteAddressBuffer::ByteAddressBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource)
        : Buffer(device, resource)
    {
    }

    ByteAddressBuffer::~ByteAddressBuffer() = default;

    size_t ByteAddressBuffer::buffer_size() const
    {
      return m_buffer_size;
    }
  } // namespace renderer
} // namespace rex