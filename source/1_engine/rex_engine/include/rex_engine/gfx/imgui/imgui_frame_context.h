#pragma once

#include "rex_engine/gfx/core/viewport.h"

#include "rex_engine/gfx/system/copy_context.h"

#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"

struct ImDrawData;

namespace rex
{
  namespace gfx
  {
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;

    // ImGui only needs to pass the mvp matrix to the shader
    struct ImGuiVertexConstantBuffer
    {
      f32 mvp[4][4];
    };

    // An frame context holds all the objects needed to render an imgui object for a frame
    class ImGuiFrameContext
    {
    public:
      ImGuiFrameContext();

      // Update the frame context's data based on the draw data and upload this to the gpu.
      ScopedPoolObject<SyncInfo> update_data(ImDrawData* drawData);

      // Return the viewport used for the frame const
      const Viewport& viewport() const;
      // Return the vertex buffer of the frame context
      VertexBuffer* vertex_buffer();
      // Return the index buffer of the frame context
      IndexBuffer* index_buffer();
      // Return the constant buffer of the frame context
      ConstantBuffer* constant_buffer();

    private:
      // Increase the vertex buffer to a new size
      void increase_vertex_buffer(s32 newSize);
      // Incrase the index buffer to a new size
      void increase_index_buffer(s32 newSize);
      // Update the frame context's data to the gpu
      ScopedPoolObject<SyncInfo> copy_buffer_data(ImDrawData* drawData);

    private:
      rsl::unique_ptr<VertexBuffer> m_vertex_buffer;
      rsl::unique_ptr<IndexBuffer> m_index_buffer;
      rsl::unique_ptr<ConstantBuffer> m_constant_buffer;
      Viewport m_viewport;
      ImGuiVertexConstantBuffer m_constant_buffer_data;
      static constexpr s32 s_buffer_increment_size = 5000;
    };
  }
}