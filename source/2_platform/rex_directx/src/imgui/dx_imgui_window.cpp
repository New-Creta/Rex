#include "rex_directx/imgui/dx_imgui_window.h"

#include "rex_directx/diagnostics/dx_call.h"

#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/gfx/dx_render_context.h"

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/gfx/render_context.h"

namespace rex
{
  namespace gfx
  {
    ImGuiWindow::ImGuiWindow(ImGuiViewport* viewport, const ImGuiDevice& creationInfo)
      : m_viewport(viewport)
    {
      // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
      // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
      HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
      IM_ASSERT(hwnd != 0);
      m_swapchain = rhi::create_swapchain(creationInfo.command_queue, creationInfo.max_num_frames_in_flight, hwnd);

      const s32 width = static_cast<s32>(viewport->Size.x);
      const s32 height = static_cast<s32>(viewport->Size.y);

      ClearStateDesc desc{};
      desc.rgba = rsl::colors::LightSteelBlue;
      desc.flags.add_state(ClearBits::ClearColorBuffer);

      m_clear_state = rsl::make_unique<ClearState>(desc);
    }

    void ImGuiWindow::render(ClearRenderTarget clearRenderTarget, Material* material)
    {
      auto render_ctx = gfx::new_render_ctx();

      // Render the imgui viewport directly on the back buffer
      RenderTarget* render_target = m_swapchain->current_buffer();

      render_ctx->transition_buffer(render_target, ResourceState::RenderTarget);
      render_ctx->set_render_target(render_target);

      if (clearRenderTarget)
      {
        render_ctx->clear_render_target(render_target, m_clear_state.get());
      }

      render_ctx->bind_material(material);
      render_ctx->set_blend_factor({ 0.0f, 0.0f, 0.0f, 0.0f });

      m_viewport.render(*render_ctx.get());
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