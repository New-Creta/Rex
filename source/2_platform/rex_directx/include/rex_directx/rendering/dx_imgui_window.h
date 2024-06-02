//#pragma once
//
//#include "rex_directx/rendering/dx_imgui_viewport.h"
//
//#include "rex_directx/system/dx_commandlist.h"
//#include "rex_directx/system/dx_swapchain.h"
//#include "rex_renderer_core/resources/root_signature.h"
//#include "rex_renderer_core/resources/pipeline_state.h"
//#include "rex_renderer_core/resources/constant_buffer.h"
//
//namespace rex
//{
//  namespace rhi
//  {
//    class RenderTarget;
//  }
//
//  namespace renderer
//  {
//    // Am imgui window is a standalone window that's just used for imgui rendering
//    // Rex uses the docking branch of imgui which supports imgui viewports getting dragged outside of the main window
//    // This classes represents such a window that got created by dragging a viewport outside of the main window.
//    class ImGuiWindow
//    {
//    public:
//      ImGuiWindow(ImGuiViewport* viewport, ID3D12CommandQueue* commandQueue, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, rhi::RootSignature* rootSignature, rhi::PipelineState* pso, rhi::ConstantBuffer* cb);
//
//      void render();
//
//      void wait_for_pending_operations();
//      void resize_buffers(s32 width, s32 height);
//      void present();
//      void yield_thread_until_in_sync_with_gpu();
//
//    private:
//      Error init_swapchain(ID3D12CommandQueue* device, s32 width, s32 height, s32 maxNumFramesInFlight, HWND hwnd, DXGI_FORMAT rtvFormat);
//
//    private:
//      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
//      rsl::unique_array<rsl::unique_ptr<rhi::RenderTarget>> m_render_targets;
//      RexImGuiViewport m_viewport;
//    };
//  }
//}