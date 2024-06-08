#pragma once

#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/rhi/viewport.h"

#include "rex_renderer_core/rhi/copy_context.h"

struct ImDrawData;

namespace rex
{
  namespace rhi
  {
    class CommandList;
    class ConstantBuffer;
    class RenderTarget;
  }

  struct ImGuiVertexConstantBuffer
  {
    f32 mvp[4][4];
  };

  class ImGuiFrameContext
  {
  public:
    ImGuiFrameContext();

    ScopedPoolObject<rhi::SyncInfo> update_data(ImDrawData* drawData);

    const rhi::Viewport& viewport() const;
    rhi::VertexBuffer* vertex_buffer();
    rhi::IndexBuffer* index_buffer();
    rhi::ConstantBuffer* constant_buffer();
    const ImGuiVertexConstantBuffer& cb_data() const;

  private:
    void increase_vertex_buffer(s32 newSize);
    void increase_index_buffer(s32 newSize);
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