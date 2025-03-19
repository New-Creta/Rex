#pragma once

#include "rex_engine/gfx/core/index_buffer_format.h"

#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class DxIndexBuffer : public IndexBuffer
    {
    public:
      DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, IndexBufferFormat format);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };

  } // namespace gfx
} // namespace rex
