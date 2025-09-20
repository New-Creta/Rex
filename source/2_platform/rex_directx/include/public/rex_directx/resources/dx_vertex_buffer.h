#pragma once

#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_directx/resources/dx_resource.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class DxVertexBuffer : public VertexBuffer, public DxResource
    {
    public:
      DxVertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numVertices, rsl::memory_size vertexSize);

    };
  } // namespace gfx
} // namespace rex