#include "rex_renderer_core/imgui/imgui_frame_context.h"

#include "rex_renderer_core/rhi/rhi.h"

#include "rex_renderer_core/gfx/graphics.h"

#include "imgui/imgui.h"

namespace rex
{
  ImGuiFrameContext::ImGuiFrameContext()
  {
    increase_vertex_buffer(5000);
    increase_index_buffer(5000);

    m_constant_buffer = rhi::create_constant_buffer(sizeof(ImGuiVertexConstantBuffer));
  }

  ScopedPoolObject<rhi::SyncInfo> ImGuiFrameContext::update_data(ImDrawData* drawData)
  {
    m_viewport.width = drawData->DisplaySize.x;
    m_viewport.height = drawData->DisplaySize.y;
    m_viewport.min_depth = 0.0f;
    m_viewport.max_depth = 1.0f;
    m_viewport.top_left_x = m_viewport.top_left_y = 0.0f;

    // Increase vertex or index buffer if needed
    if (m_vertex_buffer->count() < drawData->TotalVtxCount)
    {
      increase_vertex_buffer(drawData->TotalVtxCount);
    }
    if (m_index_buffer->count() < drawData->TotalIdxCount)
    {
      increase_index_buffer(drawData->TotalIdxCount);
    }

    return copy_buffer_data(drawData);
  }

  const rhi::Viewport& ImGuiFrameContext::viewport() const
  {
    return m_viewport;
  }
  rhi::VertexBuffer* ImGuiFrameContext::vertex_buffer()
  {
    return m_vertex_buffer.get();
  }
  rhi::IndexBuffer* ImGuiFrameContext::index_buffer()
  {
    return m_index_buffer.get();
  }
  rhi::ConstantBuffer* ImGuiFrameContext::constant_buffer()
  {
    return m_constant_buffer.get();
  }
  
  const ImGuiVertexConstantBuffer& ImGuiFrameContext::cb_data() const
  {
    return m_constant_buffer_data;
  }

  void ImGuiFrameContext::increase_vertex_buffer(s32 newSize)
  {
    m_vertex_buffer = rhi::create_vertex_buffer(newSize + s_buffer_increment_size, sizeof(ImDrawVert));
  }
  void ImGuiFrameContext::increase_index_buffer(s32 newSize)
  {
    rex::renderer::IndexBufferFormat format = sizeof(ImDrawIdx) == 2
      ? rex::renderer::IndexBufferFormat::Uint16
      : rex::renderer::IndexBufferFormat::Uint32;

    m_index_buffer = rhi::create_index_buffer(newSize + s_buffer_increment_size, format);
  }
  ScopedPoolObject<rhi::SyncInfo> ImGuiFrameContext::copy_buffer_data(ImDrawData* drawData)
  {
    auto copy_context = gfx::new_copy_ctx();

    s32 vtx_offset = 0;
    s32 idx_offset = 0;

    for (s32 n = 0; n < drawData->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = drawData->CmdLists[n];
      copy_context->update_buffer(m_vertex_buffer.get(), cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), vtx_offset);
      copy_context->update_buffer(m_index_buffer.get(), cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), idx_offset);

      vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
      idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
    }

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    {
      float L = drawData->DisplayPos.x;
      float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
      float T = drawData->DisplayPos.y;
      float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
      float mvp[4][4] =
      {
          { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
          { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
          { 0.0f,         0.0f,           0.5f,       0.0f },
          { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
      };
      memcpy(&m_constant_buffer_data.mvp, mvp, sizeof(mvp));
    }
    copy_context->update_buffer(m_constant_buffer.get(), &m_constant_buffer_data, sizeof(m_constant_buffer_data), 0);

    return copy_context->execute_on_gpu();
  }
}