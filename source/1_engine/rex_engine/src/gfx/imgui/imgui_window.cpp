#include "rex_engine/gfx/imgui/imgui_window.h"
#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/gfx/imgui/imgui_device.h"
#include "rex_engine/gfx/imgui/imgui_window_render_params.h"

#include "rex_engine/gfx/system/gal.h"
#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/rendering/render_pass.h"

namespace rex
{
  namespace gfx
  {
    // Create a new imgui window object
    void imgui_create_window(ImGuiViewport* viewport)
    {
      ImGuiWindow* imgui_window = rex::debug_alloc<ImGuiWindow>(viewport, imgui_device());
      viewport->RendererUserData = imgui_window;
    }
    // Destroy an imgui window object
    void imgui_destroy_window(ImGuiViewport* viewport)
    {
      if (ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData)
      {
        imgui_window->wait_for_pending_operations();
        rsl::destroy_at(imgui_window);
        rex::debug_dealloc(imgui_window, sizeof(ImGuiWindow));
      }
      viewport->RendererUserData = nullptr;
    }
    // Render a given imgui window
    void imgui_render_window(ImGuiViewport* viewport, void* windowParamsAsVoid)
    {
      ImGuiWindowRenderParams* render_params = static_cast<ImGuiWindowRenderParams*>(windowParamsAsVoid);
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
      const bool clear_render_target = !(viewport->Flags & ImGuiViewportFlags_NoRendererClear);

      imgui_window->render(clear_render_target, render_params);
    }
    // Resize a imgui window
    void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size)
    {
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
      imgui_window->wait_for_pending_operations();
      imgui_window->resize_buffers(static_cast<s32>(size.x), static_cast<s32>(size.y));
    }
    // Present an imgui window, swapping the back with front buffer
    void imgui_swap_buffers(ImGuiViewport* viewport, void*)
    {
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;

      imgui_window->present();
      imgui_window->yield_thread_until_in_sync_with_gpu();
    }

    ImGuiWindow::ImGuiWindow(ImGuiViewport* viewport, const ImGuiDevice& creationInfo)
      : m_viewport(viewport)
    {
      // On Windows PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
      // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
      void* handle = viewport->PlatformHandleRaw ? viewport->PlatformHandleRaw : viewport->PlatformHandle;
      IM_ASSERT(handle != nullptr);
      m_swapchain = gal()->create_swapchain(creationInfo.command_queue, creationInfo.max_num_frames_in_flight, handle);

      const s32 width = static_cast<s32>(viewport->Size.x);
      const s32 height = static_cast<s32>(viewport->Size.y);
    }

    void ImGuiWindow::render(ClearRenderTarget clearRenderTarget, ImGuiWindowRenderParams* renderParams)
    {
      auto render_ctx = gfx::new_render_ctx();

      // Render the imgui viewport directly on the back buffer
      RenderTarget* render_target = m_swapchain->current_buffer();

      renderParams->render_pass->bind_to(render_ctx.get());

      // Overwrite the render pass's default render target to the render target of this window's swapchain
      render_ctx->transition_buffer(render_target, ResourceState::RenderTarget);
      render_ctx->set_render_target(render_target);

      if (clearRenderTarget)
      {
        render_ctx->clear_render_target(render_target);
      }

      m_viewport.render(render_ctx.get(), renderParams->render_pass);
      render_ctx->transition_buffer(render_target, ResourceState::Present);
    }

    void ImGuiWindow::wait_for_pending_operations()
    {
      // Implementation pending
    }
    void ImGuiWindow::resize_buffers(s32 /*width*/, s32 /*height*/)
    {
      //m_swapchain->resize_buffers(width, height, (DXGI_SWAP_CHAIN_FLAG)0);

    }
    void ImGuiWindow::present()
    {
      m_swapchain->present();
    }
    void ImGuiWindow::yield_thread_until_in_sync_with_gpu()
    {
      //while (m_command_queue->has_reached_latest_fence())
      //{
      //  ::SwitchToThread();
      //}
    }
  }
}