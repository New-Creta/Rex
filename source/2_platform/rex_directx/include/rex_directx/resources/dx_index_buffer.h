#pragma once

#include "rex_directx/system/dx_resource.h"

#include "rex_renderer_core/rendering/index_buffer_format.h"

namespace rex
{
  namespace rhi
  {
    class IndexBuffer : public Resource2
    {
    public:
      IndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, renderer::IndexBufferFormat format)
        : Resource2(resource)
        , m_num_indices(numIndices)
        , m_index_format(format)
      {}

      s32 num_indices() const
      {
        return m_num_indices;
      }

      s32 total_size() const
      {
        return m_num_indices * renderer::index_format_size(m_index_format);
      }

      renderer::IndexBufferFormat format() const
      {
        return m_index_format;
      }

    private:
      s32 m_num_indices;
      renderer::IndexBufferFormat m_index_format;
    };

  } // namespace rhi
} // namespace rex
