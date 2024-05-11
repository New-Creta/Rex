#include "rex_directx/rendering/dx_imgui_viewport.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/resources/dx_shader_program_resource.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/rendering/scissor_rect.h"

namespace rex
{
  namespace renderer
  {
    RexImGuiViewport::RexImGuiViewport(ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb)
      : m_imgui_viewport(viewport)
      , m_max_num_frames_in_flight(maxNumFramesInFlight)
      , m_frame_idx(0)
      , m_shader_program(shaderProgram)
      , m_pipeline_state(pso)
      , m_constant_buffer(cb)
    {
      // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
      // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
      HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
      IM_ASSERT(hwnd != 0);

      init_frame_contexts(device);
    }

    void RexImGuiViewport::draw(rhi::CommandList* ctx)
    {
      render_draw_data(ctx);
    }

    Error RexImGuiViewport::init_frame_contexts(ID3D12Device1* device)
    {
      // Create command allocator.
      m_frame_ctx = rsl::make_unique<rsl::unique_ptr<ImGuiFrameContext>[]>(m_max_num_frames_in_flight);
      m_render_buffers = rsl::make_unique<rsl::unique_ptr<ImGuiRenderBuffer>[]>(m_max_num_frames_in_flight);
      for (UINT i = 0; i < m_max_num_frames_in_flight; ++i)
      {
        m_frame_ctx[i] = rsl::make_unique<ImGuiFrameContext>(device);
        m_render_buffers[i] = rsl::make_unique<ImGuiRenderBuffer>();
      }

      return Error::no_error();
    }
    ImGuiRenderBuffer* RexImGuiViewport::current_render_buffer()
    {
      return m_render_buffers[m_frame_idx % m_max_num_frames_in_flight].get();
    }

    void RexImGuiViewport::update_to_next_frame_ctx()
    {
      ++m_frame_idx;
      if (m_frame_idx == m_max_num_frames_in_flight)
      {
        m_frame_idx = 0;
      }
    }
    ImGuiFrameContext* RexImGuiViewport::current_frame_ctx()
    {
      return m_frame_ctx[m_frame_idx].get();
    }

    void RexImGuiViewport::setup_render_state(ImDrawData* drawData, rhi::CommandList* ctx, class ImGuiRenderBuffer* fr)
    {
      // Setup orthographic projection matrix into our constant buffer
      // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
      ImGuiVertexConstantBuffer vertex_constant_buffer;
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
        memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
      }

      // Setup viewport
      Viewport vp{};
      vp.width = drawData->DisplaySize.x;
      vp.height = drawData->DisplaySize.y;
      vp.min_depth = 0.0f;
      vp.max_depth = 1.0f;
      vp.top_left_x = vp.top_left_y = 0.0f;

      rex::rhi::set_viewport(ctx, vp);
      rex::rhi::set_vertex_buffer(ctx, fr->vertex_buffer);
      rex::rhi::set_index_buffer(ctx, fr->index_buffer);
      rex::rhi::set_primitive_topology(ctx, PrimitiveTopology::TriangleList);
      rex::rhi::set_pso(ctx, m_pipeline_state);
      rex::rhi::set_shader(ctx, m_shader_program);
      rex::rhi::update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer), ctx->dx_object());
      rex::rhi::set_constant_buffer(ctx, 0, m_constant_buffer);

      ctx->set_viewport(vp);
      ctx->set_vertex_buffer(fr->vertex_buffer);
      ctx->set_index_buffer(fr->index_buffer);
      ctx->set_primitive_topology(PrimitiveTopology::TriangleList);
      ctx->set_pso(m_pipeline_state);
      ctx->set_shader(m_shader_program);
      ctx->update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer));
      ctx->set_constant_buffer(0, m_constant_buffer);
      ctx->set_blend_factor(blend_factor);





      // Setup blend factor
      const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
      rex::rhi::set_blend_factor(ctx, blend_factor);
    }
    void RexImGuiViewport::render_draw_data(rhi::CommandList* ctx)
    {
      ImDrawData* draw_data = m_imgui_viewport->DrawData;

      // Avoid rendering when minimized
      if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

      if (draw_data->CmdListsCount == 0)
        return;

      update_to_next_frame_ctx();

      ImGuiRenderBuffer* render_buffer = current_render_buffer();

      // Upload vertex/index data into a single contiguous GPU buffer
      update_render_buffer(ctx, draw_data, render_buffer);

      // Setup desired DX state
      setup_render_state(draw_data, ctx, render_buffer);

      // Render command lists
      // (Because we merged all buffers into a single one, we maintain our own offset into them)
      s32 global_vtx_offset = 0;
      s32 global_idx_offset = 0;
      ImVec2 clip_off = draw_data->DisplayPos;
      for (s32 n = 0; n < draw_data->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
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
          
          D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
          texture_handle.ptr = (UINT64)pcmd->GetTexID();

          rhi::set_scissor_rect(ctx, rect);
          rhi::set_graphics_root_descriptor_table(ctx, texture_handle);
          rhi::draw_indexed_instanced(ctx, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);

          ctx->set_scissor_rect(rect);
          ctx->set_graphics_root_descriptor_table(texture_handle);
          ctx->draw_indexed_instanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
      }
    }

    void RexImGuiViewport::increase_vertex_buffer(ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer)
    {
      renderBuffer->VertexBufferSize = drawData->TotalVtxCount + 5000;
      renderBuffer->vertex_buffer = rex::rhi::create_vertex_buffer(sizeof(ImDrawVert) * renderBuffer->VertexBufferSize, sizeof(ImDrawVert));
    }
    void RexImGuiViewport::increase_index_buffer(ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer)
    {
      rex::renderer::IndexBufferFormat format = sizeof(ImDrawIdx) == 2
        ? rex::renderer::IndexBufferFormat::Uint16
        : rex::renderer::IndexBufferFormat::Uint32;
      renderBuffer->IndexBufferSize = drawData->TotalIdxCount + 10000;
      renderBuffer->index_buffer = rex::rhi::create_index_buffer(renderBuffer->IndexBufferSize * sizeof(ImDrawIdx), format);
    }

    void RexImGuiViewport::update_render_buffer(rhi::CommandList* ctx, ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer)
    {
      // Create and grow vertex/index buffers if needed
      if (!renderBuffer->vertex_buffer.is_valid() || renderBuffer->VertexBufferSize < drawData->TotalVtxCount)
      {
        increase_vertex_buffer(drawData, renderBuffer);
      }
      if (!renderBuffer->index_buffer.is_valid() || renderBuffer->IndexBufferSize < drawData->TotalIdxCount)
      {
        increase_index_buffer(drawData, renderBuffer);
      }

      s32 vtx_offset = 0;
      s32 idx_offset = 0;

      for (s32 n = 0; n < drawData->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        rex::rhi::update_buffer(renderBuffer->vertex_buffer, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), ctx->dx_object(), vtx_offset);
        rex::rhi::update_buffer(renderBuffer->index_buffer, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), ctx->dx_object(), idx_offset);
        vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
      }
    }

  }
}