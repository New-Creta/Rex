#pragma once

#include "rex_directx/rendering/dx_imgui_viewport.h"

#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_root_signature.h"
#include "rex_directx/system/dx_pipeline_state.h"
#include "rex_directx/system/dx_constant_buffer.h"

namespace rex
{
  namespace rhi
  {
    class RenderTarget;
  }

  namespace renderer
  {
    // Am imgui window is a standalone window that's just used for imgui rendering
    // Rex uses the docking branch of imgui which supports imgui viewports getting dragged outside of the main window
    // This classes represents such a window that got created by dragging a viewport outside of the main window.
    class ImGuiWindow
    {
    public:
      ImGuiWindow(ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, rhi::RootSignature* rootSignature, rhi::PipelineState* pso, rhi::ConstantBuffer* cb);

      void begin_draw();
      void draw();
      void end_draw();

      void wait_for_pending_operations();
      void clear_render_target(const ImVec4& clearColor);
      void resize_buffers(s32 width, s32 height);
      void present();
      void yield_thread_until_in_sync_with_gpu();

    private:
      Error init_command_queue(ID3D12Device1* device);
      Error init_desc_heap(ID3D12Device1* device, s32 maxNumFramesInFlight);
      Error init_cmd_list(ID3D12Device1* device);
      Error init_swapchain(ID3D12Device1* device, s32 width, s32 height, s32 maxNumFramesInFlight, HWND hwnd, DXGI_FORMAT rtvFormat);

    private:
      rsl::unique_ptr<rhi::CommandList> m_command_list; 
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
      //rsl::unique_ptr<rhi::DescriptorHeap> m_rtv_desc_heap;
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue; // Check if I need a command queue for this swapchain
      rsl::unique_ptr<rhi::ClearStateResource> m_clear_state;
      rsl::unique_array<rsl::unique_ptr<ImGuiRenderBuffer>> m_render_buffers;

      rsl::unique_array<rsl::unique_ptr<rhi::RenderTarget>> m_render_targets;

      RexImGuiViewport m_viewport;
    };
  }
}