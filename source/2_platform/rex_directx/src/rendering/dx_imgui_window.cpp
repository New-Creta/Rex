//#include "rex_directx/rendering/dx_imgui_window.h"
//
//#include "rex_directx/diagnostics/dx_call.h"
//#include "rex_directx/rendering/dx_imgui_viewport.h"
//#include "rex_directx/resources/dx_clear_state_resource.h"
//
//#include "rex_directx/system/dx_rhi.h"
//
//#include "rex_renderer_core/gfx/graphics.h"
//#include "rex_renderer_core/rhi/render_context.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    ImGuiWindow::ImGuiWindow(ImGuiViewport* viewport, ID3D12CommandQueue* commandQueue, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, rhi::RootSignature* rootSignature, rhi::PipelineState* pso, rhi::ConstantBuffer* cb)
//      : m_viewport(viewport, maxNumFramesInFlight, rtvFormat, rootSignature, pso, cb)
//    {
//      // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
//      // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
//      HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
//      IM_ASSERT(hwnd != 0);
//      m_swapchain = rhi::create_swapchain(commandQueue, maxNumFramesInFlight, hwnd);
//
//      const s32 width = viewport->Size.x;
//      const s32 height = viewport->Size.y;
//
//      init_cmd_list(device);
//      init_swapchain(device, width, height, maxNumFramesInFlight, hwnd, rtvFormat);
//
//      m_render_targets = rsl::make_unique<rsl::unique_ptr<rhi::RenderTarget>[]>(maxNumFramesInFlight);
//      for (s32 i = 0; i < maxNumFramesInFlight; ++i)
//      {
//        rhi::Resource2* buffer = m_swapchain->get_buffer(i);
//        m_render_targets[i] = rhi::create_render_target_from_backbuffer(buffer);
//      }
//
//      rhi::ClearStateDesc desc{};
//      desc.rgba = rsl::colors::LightSteelBlue;
//      desc.flags.add_state(ClearBits::ClearColorBuffer);
//
//      m_clear_state = rsl::make_unique<rhi::ClearStateResource>(desc);
//    }
//
//    void ImGuiWindow::render()
//    {
//      auto render_ctx = gfx::new_render_ctx();
//
//      rhi::RenderTarget* render_target = current_render_target();
//
//      render_ctx->transition_buffer(render_target, rhi::ResourceState::RenderTarget);
//      render_ctx->set_render_target(render_target);
//      m_viewport.draw(render_ctx.get());
//      render_ctx->transition_buffer(render_target, rhi::ResourceState::Present);
//
//      advance_to_next_frame();
//    }
//
//    void ImGuiWindow::clear_render_target(const ImVec4& clearColor)
//    {
//      rhi::RenderTarget* render_target = m_render_targets[m_swapchain->get()->GetCurrentBackBufferIndex()].get();
//      m_command_list->clear_render_target(render_target, m_clear_state.get());
//    }
//
//    void ImGuiWindow::wait_for_pending_operations()
//    {
//    }
//    void ImGuiWindow::resize_buffers(s32 width, s32 height)
//    {
//      m_swapchain->resize_buffers(width, height, (DXGI_SWAP_CHAIN_FLAG)0);
//
//    }
//    void ImGuiWindow::present()
//    {
//      m_swapchain->get()->Present(0, 0);
//    }
//    void ImGuiWindow::yield_thread_until_in_sync_with_gpu()
//    {
//      //while (m_command_queue->has_reached_latest_fence())
//      //{
//      //  ::SwitchToThread();
//      //}
//    }
//
//    Error ImGuiWindow::init_cmd_list(ID3D12Device1* device)
//    {
//      // Create command list.
//      m_command_list = rhi::create_commandlist();
//
//      return Error::no_error();
//    }
//    Error ImGuiWindow::init_swapchain(ID3D12Device1* device, s32 width, s32 height, s32 maxNumFramesInFlight, HWND hwnd, DXGI_FORMAT rtvFormat)
//    {
//      m_swapchain = rhi::create_swapchain(maxNumFramesInFlight, hwnd);
//
//      return Error::no_error();
//    }
//
//  }
//}