#include "rex_directx/resources/index_buffer.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    IndexBuffer::IndexBuffer(Device& device, s64 numIndices, DXGI_FORMAT indexFormat)
        : Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(numIndices * (indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4)))
        , m_num_indicies(numIndices)
        , m_index_format(indexFormat)
        , m_index_buffer_view {}
    {
      REX_ASSERT_X(indexFormat == DXGI_FORMAT_R16_UINT || indexFormat == DXGI_FORMAT_R32_UINT, "Invalid index format");
      create_index_buffer_view();
    }

    IndexBuffer::IndexBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource, s64 numIndices, DXGI_FORMAT indexFormat)
        : Buffer(device, resource)
        , m_num_indicies(numIndices)
        , m_index_format(indexFormat)
        , m_index_buffer_view {}
    {
      REX_ASSERT_X(indexFormat == DXGI_FORMAT_R16_UINT || indexFormat == DXGI_FORMAT_R32_UINT, "Invalid index format");
      create_index_buffer_view();
    }

    IndexBuffer::~IndexBuffer() = default;

    s64 IndexBuffer::num_indices() const
    {
      return m_num_indicies;
    }

    DXGI_FORMAT IndexBuffer::index_format() const
    {
      return m_index_format;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBuffer::index_buffer_view() const
    {
      return m_index_buffer_view;
    }

    void IndexBuffer::create_index_buffer_view()
    {
      s64 buffer_size = m_num_indicies * (m_index_format == DXGI_FORMAT_R16_UINT ? 2 : 4);

      m_index_buffer_view.BufferLocation = d3d_resource()->GetGPUVirtualAddress();
      m_index_buffer_view.SizeInBytes    = static_cast<u32>(buffer_size);
      m_index_buffer_view.Format         = m_index_format;
    }
  } // namespace renderer
} // namespace rex