#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"

namespace rex
{
  namespace renderer
  {
    struct ImGuiVertexConstantBuffer
    {
      f32   mvp[4][4];
    };

    // Buffers used during the rendering of a frame
    struct ImGuiRenderBuffer
    {
      s32                 IndexBufferSize;    // the number of indices the index buffer supports
      s32                 VertexBufferSize;   // the number of vertices the vertex buffer supports

      rsl::unique_ptr<rhi::VertexBuffer> vertex_buffer;   // resource slot of the vertex buffer
      rsl::unique_ptr<rhi::IndexBuffer> index_buffer;    // resource slot of the index buffer
    };
  }
}