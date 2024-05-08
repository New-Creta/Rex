#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_directx/utility/dx_util.h"
#include "imgui/imgui.h"
#include "rex_std/memory.h"

#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_commandlist2.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_directx/rendering/dx_imgui_frame_context.h"
#include "rex_directx/system/dx_commandlist2.h"

namespace rex
{
  namespace renderer
  {
    // Wrapper around a imgui viewport.
    class RexImGuiViewport
    {
    public:
      RexImGuiViewport(ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb);

      void draw(rhi::CommandList2* ctx);
      ImGuiFrameContext* current_frame_ctx();

    protected:
      void update_to_next_frame_ctx();

      void setup_render_state(ImDrawData* drawData, rhi::CommandList2* ctx, class ImGuiRenderBuffer* fr);
      void render_draw_data(rhi::CommandList2* ctx);

    private:
      Error init_frame_contexts(ID3D12Device1* device);
      ImGuiRenderBuffer* current_render_buffer();

      void increase_vertex_buffer(ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer);
      void increase_index_buffer(ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer);

      void update_render_buffer(rhi::CommandList2* ctx, ImDrawData* drawData, ImGuiRenderBuffer* renderBuffer);

    private:
      rsl::unique_array<rsl::unique_ptr<ImGuiFrameContext>> m_frame_ctx;
      rsl::unique_array<rsl::unique_ptr<ImGuiRenderBuffer>> m_render_buffers;
      ImGuiViewport* m_imgui_viewport;

      rex::rhi::ResourceSlot m_shader_program;
      rex::rhi::ResourceSlot m_pipeline_state;
      rex::rhi::ResourceSlot m_constant_buffer;

      s32 m_max_num_frames_in_flight;
      s32 m_frame_idx;
    };
  }
}