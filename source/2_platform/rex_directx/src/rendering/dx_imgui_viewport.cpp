#include "rex_directx/rendering/dx_imgui_viewport.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/resources/dx_shader_program_resource.h"

namespace rex
{
  namespace renderer
  {
    ImGuiViewport::ImGuiViewport(::ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb)
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

    void ImGuiViewport::draw(ID3D12GraphicsCommandList* ctx)
    {
      render_draw_data(ctx);
    }

    Error ImGuiViewport::init_frame_contexts(ID3D12Device1* device)
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

    void ImGuiViewport::update_to_next_frame_ctx()
    {
      ++m_frame_idx;
      if (m_frame_idx == m_max_num_frames_in_flight)
      {
        m_frame_idx = 0;
      }
    }
    ImGuiFrameContext* ImGuiViewport::current_frame_ctx()
    {
      return m_frame_ctx[m_frame_idx].get();
    }

    void ImGuiViewport::setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, class ImGuiRenderBuffer* fr)
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
      D3D12_VIEWPORT vp;
      memset(&vp, 0, sizeof(D3D12_VIEWPORT));
      vp.Width = drawData->DisplaySize.x;
      vp.Height = drawData->DisplaySize.y;
      vp.MinDepth = 0.0f;
      vp.MaxDepth = 1.0f;
      vp.TopLeftX = vp.TopLeftY = 0.0f;
      ctx->RSSetViewports(1, &vp);

      rex::rhi::set_vertex_buffer(fr->vertex_buffer, ctx);
      rex::rhi::set_index_buffer(fr->index_buffer, ctx);

      ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      ctx->SetPipelineState(rex::rhi::get_pso(m_pipeline_state)->get());
      ctx->SetGraphicsRootSignature(rex::rhi::get_shader(m_shader_program)->root_signature());

      rex::rhi::update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer), ctx);
      rex::rhi::set_constant_buffer(0, m_constant_buffer, ctx);

      // Setup blend factor
      const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
      ctx->OMSetBlendFactor(blend_factor);

      rex::rhi::set_vertex_buffer(ctx, fr->vertex_buffer);
      rex::rhi::set_index_buffer(ctx, fr->index_buffer);
      rex::rhi::set_primitive_topology(ctx, PrimitiveTopology::TriangleList);
      rex::rhi::set_pso(ctx, m_pipeline_state);
      rex::rhi::set_shader(ctx, m_shader_program);
      rex::rhi::update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer), ctx);
      rex::rhi::set_constant_buffer(ctx, 0, m_constant_buffer);
      rex::rhi::set_blend_factor(ctx, blend_factor);
    }
    void ImGuiViewport::render_draw_data(ID3D12GraphicsCommandList* ctx)
    {
      ImDrawData* drawData = m_imgui_viewport->DrawData;

      // Avoid rendering when minimized
      if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
        return;

      if (drawData->CmdListsCount == 0)
        return;

      ImGuiViewport* vd = this;
      update_to_next_frame_ctx();
      ImGuiRenderBuffer* fr = m_render_buffers[m_frame_idx % m_max_num_frames_in_flight].get();

      // Create and grow vertex/index buffers if needed
      if (!fr->vertex_buffer.is_valid() || fr->VertexBufferSize < drawData->TotalVtxCount)
      {
        fr->VertexBufferSize = drawData->TotalVtxCount + 5000;
        fr->vertex_buffer = rex::rhi::create_vertex_buffer(sizeof(ImDrawVert) * fr->VertexBufferSize, sizeof(ImDrawVert));
      }
      if (!fr->index_buffer.is_valid() || fr->IndexBufferSize < drawData->TotalIdxCount)
      {
        rex::renderer::IndexBufferFormat format = sizeof(ImDrawIdx) == 2
          ? rex::renderer::IndexBufferFormat::Uint16
          : rex::renderer::IndexBufferFormat::Uint32;
        fr->IndexBufferSize = drawData->TotalIdxCount + 10000;
        fr->index_buffer = rex::rhi::create_index_buffer(fr->IndexBufferSize * sizeof(ImDrawIdx), format);
      }

      // Upload vertex/index data into a single contiguous GPU buffer
      s32 vtx_offset = 0;
      s32 idx_offset = 0;

      for (int n = 0; n < drawData->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        rex::rhi::update_buffer(fr->vertex_buffer, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), ctx, vtx_offset);
        rex::rhi::update_buffer(fr->index_buffer, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), ctx, idx_offset);
        vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
      }

      // Setup desired DX state
      setup_render_state(drawData, ctx, fr);

      // Render command lists
      // (Because we merged all buffers into a single one, we maintain our own offset into them)
      int global_vtx_offset = 0;
      int global_idx_offset = 0;
      ImVec2 clip_off = drawData->DisplayPos;
      for (int n = 0; n < drawData->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
          const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
          if (pcmd->UserCallback != nullptr)
          {
            // User callback, registered via ImDrawList::AddCallback()
            // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
            if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
              setup_render_state(drawData, ctx, fr);
            else
              pcmd->UserCallback(cmd_list, pcmd);
          }
          else
          {
            // Project scissor/clipping rectangles into framebuffer space
            ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
            ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
            if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
              continue;

            // Apply Scissor/clipping rectangle, Bind texture, Draw
            const D3D12_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
            D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
            texture_handle.ptr = (UINT64)pcmd->GetTexID();
            ctx->SetGraphicsRootDescriptorTable(1, texture_handle);
            ctx->RSSetScissorRects(1, &r);
            ctx->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);


            rex::rhi::set_graphics_root_desc_table(ctx, 1, texture_handle);
            rex::rhi::set_scissor_rect(1, r);
            rex::rhi::draw_indexed_instanced(ctx, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
          }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
      }
    }
  }
}