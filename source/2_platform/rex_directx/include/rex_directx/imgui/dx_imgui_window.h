#pragma once

#include "rex_directx/system/dx_swapchain.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/imgui/imgui_viewport.h"
#include "rex_renderer_core/imgui/imgui_resources.h"
#include "rex_renderer_core/imgui/imgui_device.h"

#include "imgui/imgui.h"


#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    class RenderTarget;
  }

  namespace gfx
  {
    DEFINE_YES_NO_ENUM(ClearRenderTarget);

    // Am imgui window is a standalone window that's just used for imgui rendering
    // Rex uses the docking branch of imgui which supports imgui viewports getting dragged outside of the main window
    // This classes represents such a window that got created by dragging a viewport outside of the main window.
    class ImGuiWindow
    {
    public:
      ImGuiWindow(ImGuiViewport* viewport, const ImGuiDevice& creationInfo);

      // Render the imgui window
      void render(ClearRenderTarget clearRenderTarget, Material* material);

      // Wait for gpu commands to complete, this is blocking
      void wait_for_pending_operations();
      // Resize the swapchains buffers
      void resize_buffers(s32 width, s32 height);
      // Present the backbuffer by swapping it with the front buffer
      void present();
      // Yield the thread until all gpu commands have finished executing
      void yield_thread_until_in_sync_with_gpu();

    private:
      rsl::unique_ptr<Swapchain> m_swapchain;
      rsl::unique_ptr<ClearState> m_clear_state;
      RexImGuiViewport m_viewport;
    };
  }
}