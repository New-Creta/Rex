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

namespace rex
{
  namespace renderer
  {
    class ImGuiFrameContext;

    struct ImGuiVertexConstantBuffer
    {
      float   mvp[4][4];
    };

    // Helper structure we store in the void* RendererUserData field of each ImGuiViewport to easily retrieve our backend data.
    // Main viewport created by application will only use the Resources field.
    // Secondary viewports created by this backend will use all the fields (including Window fields),
    class ImGuiViewport
    {
    public:
      ImGuiViewport(::ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb);

      void wait_for_pending_operations();

      void begin_draw();
      void end_draw();

      void clear_render_target(const ImVec4& clearColor);
      void draw(ImDrawData* drawData, rex::rhi::DescriptorHeap* srvDescHeap);

      void resize_buffers(s32 width, s32 height);
      void present();
      void yield_thread();

    private:
      Error init_command_queue(ID3D12Device1* device);
      Error init_frame_contexts(ID3D12Device1* device);
      Error init_cmd_list(ID3D12Device1* device);
      Error init_desc_heap(ID3D12Device1* device);
      Error init_swapchain(::ImGuiViewport* viewport, ID3D12Device1* device, HWND hwnd, DXGI_FORMAT rtvFormat);
      void update_to_next_frame_ctx();
      ImGuiFrameContext* current_frame_ctx();

      void setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, class ImGuiRenderBuffer* fr);
      void render_draw_data();

    private:
      rsl::unique_ptr<rhi::CommandList2> m_command_list;
      rsl::unique_array<rsl::unique_ptr<ImGuiFrameContext>> m_frame_ctx;
      rsl::unique_array<rsl::unique_ptr<ImGuiRenderBuffer>> m_render_buffers;
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      rsl::unique_ptr<rhi::DescriptorHeap> m_descriptor_heap;
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
      ::ImGuiViewport* m_imgui_viewport;

      rex::rhi::ResourceSlot m_shader_program;
      rex::rhi::ResourceSlot m_pipeline_state;
      rex::rhi::ResourceSlot m_constant_buffer;

      s64 m_fence_signaled_value;
      s32 m_max_num_frames_in_flight;
      s32 m_frame_idx;
    };
  }
}