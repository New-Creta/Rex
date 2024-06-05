#include "rex_renderer_core/imgui/imgui_viewport.h"

#include "rex_renderer_core/rhi/viewport.h"
#include "rex_renderer_core/rhi/scissor_rect.h"
#include "rex_renderer_core/rhi/primitive_topology.h"
#include "rex_renderer_core/rhi/resource_state.h"

#include "rex_renderer_core/system/copy_engine.h"
#include "rex_renderer_core/system/command_list.h"

#include "rex_renderer_core/resources/constant_buffer.h"

#include "rex_renderer_core/gfx/graphics.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogImGui2);

  RexImGuiViewport::RexImGuiViewport(ImGuiViewport* imguiViewport, ImGuiRenderState renderState)
    : m_imgui_viewport(imguiViewport)
    , m_render_state(renderState)
  {
    for (s32 i = 0; i < 3; ++i)
    {
      m_frame_contexts.emplace_back();
    }
  }

  void RexImGuiViewport::render(rhi::RenderContext& renderContext)
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
    ScopedPoolObject<rhi::SyncInfo> sync_info = frame_ctx.update_data(draw_data, m_render_state.constant_buffer);

    // Wait for the data to be updated on the gpu before we start executing render commands
    renderContext.stall(*sync_info);

    // Setup the render state, prepare it for rendering
    setup_render_state(renderContext, draw_data, frame_ctx);

    // Draw all the primitives
    draw(renderContext, draw_data);

    // Adavance to the next frame context so we can fill its data during the next frame
    advance_frame_ctx();
  }

  ImGuiFrameContext& RexImGuiViewport::current_frame_ctx()
  {
    return m_frame_contexts[m_frame_idx];
  }

  void RexImGuiViewport::advance_frame_ctx()
  {
    ++m_frame_idx;
    if (m_frame_idx == m_frame_contexts.size())
    {
      m_frame_idx = 0;
    }
  }

  void RexImGuiViewport::setup_render_state(rhi::RenderContext& ctx, ImDrawData* drawData, ImGuiFrameContext& frameCtx)
  {
    ctx.transition_buffer(frameCtx.vertex_buffer(), rhi::ResourceState::VertexAndConstantBuffer);
    ctx.transition_buffer(frameCtx.index_buffer(), rhi::ResourceState::IndexBuffer);
    ctx.transition_buffer(m_render_state.constant_buffer, rhi::ResourceState::VertexAndConstantBuffer);

    ctx.set_viewport(frameCtx.viewport());
    ctx.set_vertex_buffer(frameCtx.vertex_buffer());
    ctx.set_index_buffer(frameCtx.index_buffer());
    ctx.set_primitive_topology(m_render_state.primitive_topology);
    ctx.set_pipeline_state(m_render_state.pso);
    ctx.set_root_signature(m_render_state.root_signature);
    ctx.set_constant_buffer(0, m_render_state.constant_buffer);
    ctx.set_blend_factor(m_render_state.blend_factor.data());

    //ctx.wait_for_finish();


    //cmdList->transition_buffer(frameCtx.vertex_buffer(), ResourceState::VertexAndConstantBuffer);
    //cmdList->transition_buffer(frameCtx.index_buffer(), ResourceState::IndexBuffer);
    //cmdList->transition_buffer(m_render_state.constant_buffer, ResourceState::VertexAndConstantBuffer);

    //cmdList->set_viewport(frameCtx.viewport());
    //cmdList->set_vertex_buffer(frameCtx.vertex_buffer());
    //cmdList->set_index_buffer(frameCtx.index_buffer());
    //cmdList->set_primitive_topology(m_render_state.primitive_topology);
    //cmdList->set_pipeline_state(m_render_state.pso);
    //cmdList->set_root_signature(m_render_state.root_signature);
    //cmdList->set_constant_buffer(0, m_render_state.constant_buffer);
    //cmdList->set_blend_factor(m_render_state.blend_factor.data());
  }

  void RexImGuiViewport::draw(rhi::RenderContext& ctx, ImDrawData* drawData)
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
        rhi::ScissorRect rect{};
        rect.left = clip_min.x;
        rect.top = clip_min.y;
        rect.right = clip_max.x;
        rect.bottom = clip_max.y;

        rhi::Texture2D* texture = (rhi::Texture2D*)pcmd->GetTexID();

        ctx.set_scissor_rect(rect);
        ctx.bind_texture(texture);

        ctx.draw_indexed_instanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);

        //cmdList->set_scissor_rect(rect);
        //cmdList->bind_texture(texture);
        //cmdList->set_graphics_root_descriptor_table(1, (UINT64)texture->gpu_handle().ptr);
        //cmdList->draw_indexed_instanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
      }
      global_idx_offset += cmd_list->IdxBuffer.Size;
      global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
  }
}