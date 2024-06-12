#pragma once

#include "rex_renderer_core/rhi/viewport.h"

#include "rex_renderer_core/rhi/copy_context.h"

struct ImDrawData;

namespace rex
{
  namespace rhi
  {
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;
  }

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
    ScopedPoolObject<rhi::SyncInfo> update_data(ImDrawData* drawData);

    // Return the viewport used for the frame const
    const rhi::Viewport& viewport() const;
    // Return the vertex buffer of the frame context
    rhi::VertexBuffer* vertex_buffer();
    // Return the index buffer of the frame context
    rhi::IndexBuffer* index_buffer();
    // Return the constant buffer of the frame context
    rhi::ConstantBuffer* constant_buffer();

  private:
    // Increase the vertex buffer to a new size
    void increase_vertex_buffer(s32 newSize);
    // Incrase the index buffer to a new size
    void increase_index_buffer(s32 newSize);
    // Update the frame context's data to the gpu
    ScopedPoolObject<rhi::SyncInfo> copy_buffer_data(ImDrawData* drawData);

  private:
    rsl::unique_ptr<rhi::VertexBuffer> m_vertex_buffer;
    rsl::unique_ptr<rhi::IndexBuffer> m_index_buffer;
    rsl::unique_ptr<rhi::ConstantBuffer> m_constant_buffer;
    rhi::Viewport m_viewport;
    ImGuiVertexConstantBuffer m_constant_buffer_data;
    static constexpr s32 s_buffer_increment_size = 5000;
  };
}