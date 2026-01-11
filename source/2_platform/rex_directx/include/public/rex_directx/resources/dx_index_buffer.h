#pragma once

#include "rex_engine/gfx/core/index_buffer_format.h"

#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/resources/dx_resource.h"

namespace rex
{
  namespace gfx
  {
    class DxIndexBuffer : public IndexBuffer, public DxResource
    {
    public:
      DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, IndexBufferFormat format);

      void* api_object() const override;

    };

  } // namespace gfx
} // namespace rex
