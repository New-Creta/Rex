#include "rex_renderer_core/imgui/imgui_viewport.h"

#include "rex_renderer_core/gfx/viewport.h"
#include "rex_renderer_core/gfx/scissor_rect.h"
#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/gfx/resource_state.h"

#include "rex_renderer_core/resources/constant_buffer.h"

#include "rex_renderer_core/gfx/graphics.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  namespace gfx
  {
    RexImGuiViewport::RexImGuiViewport(ImGuiViewport* imguiViewport)
      : m_imgui_viewport(imguiViewport)
      , m_frame_idx(0)
    {
    }

    // Render the viewport using the given render context to queue gpu commands to
    void RexImGuiViewport::render(RenderContext& renderContext)
    {
      ImDrawData* draw_data = m_imgui_viewport->DrawData;

      // Avoid rendering when minimized
      if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

      // Avoid rendering when there's nothing to render
      if (draw_data->CmdListsCount == 0)
        return;

      // Update the current frame context with the data for this frame
      ImGuiFrameContext& frame_ctx = current_frame_ctx();
      ScopedPoolObject<SyncInfo> sync_info = frame_ctx.update_data(draw_data);

      // Wait for the data to be updated on the gpu before we start executing render commands
      renderContext.stall(*sync_info);

      // Setup the render state, prepare it for rendering
      setup_render_state(renderContext, frame_ctx);

      // Draw all the primitives
      draw(renderContext, draw_data);

      // Adavance to the next frame context so we can fill its data during the next frame
      advance_frame_ctx();
    }

    // Get the current frame context to use to render the viewport
    ImGuiFrameContext& RexImGuiViewport::current_frame_ctx()
    {
      return m_frame_contexts[m_frame_idx];
    }
    // Advance the index so it points to the next frame context
    void RexImGuiViewport::advance_frame_ctx()
    {
      ++m_frame_idx;
      if (m_frame_idx == m_frame_contexts.size())
      {
        m_frame_idx = 0;
      }
    }

    // Setup the render state of the viewport, it for rendering
    void RexImGuiViewport::setup_render_state(RenderContext& ctx, ImGuiFrameContext& frameCtx)
    {
      // Transition our buffers into a state so they can be used by the render pipeline
      ctx.transition_buffer(frameCtx.vertex_buffer(), ResourceState::VertexAndConstantBuffer);
      ctx.transition_buffer(frameCtx.index_buffer(), ResourceState::IndexBuffer);
      ctx.transition_buffer(frameCtx.constant_buffer(), ResourceState::VertexAndConstantBuffer);

      ctx.set_viewport(frameCtx.viewport());
      ctx.set_vertex_buffer(frameCtx.vertex_buffer());
      ctx.set_index_buffer(frameCtx.index_buffer());
      ctx.set_constant_buffer(s_constant_buffer_param_idx, frameCtx.constant_buffer());
    }

    // Draw the current viewport
    void RexImGuiViewport::draw(RenderContext& ctx, ImDrawData* drawData)
    {
      s32 global_vtx_offset = 0;
      s32 global_idx_offset = 0;
      ImVec2 clip_off = drawData->DisplayPos;
      for (s32 n = 0; n < drawData->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        for (s32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
          const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

          // Project scissor/clipping rectangles into framebuffer space
          ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
          ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
          if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
          {
            continue;
          }

          // Apply Scissor/clipping rectangle, Bind texture, Draw
          ScissorRect rect{};
          rect.left = clip_min.x;
          rect.top = clip_min.y;
          rect.right = clip_max.x;
          rect.bottom = clip_max.y;

          ctx.set_scissor_rect(rect);
          ctx.draw_indexed_instanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
      }
    }
  }
}