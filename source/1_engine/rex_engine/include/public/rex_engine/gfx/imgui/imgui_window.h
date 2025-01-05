#pragma once

#include "rex_engine/gfx/system/swapchain.h"
#include "rex_engine/gfx/resources/root_signature.h"
#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/imgui/imgui_viewport.h"
#include "rex_engine/gfx/imgui/imgui_device.h"
#include "rex_engine/gfx/imgui/imgui_window_render_params.h"

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
    // Create a new imgui window object
    void imgui_create_window(ImGuiViewport* viewport);
    // Destroy an imgui window object
    void imgui_destroy_window(ImGuiViewport* viewport);
    // Render a given imgui window
    void imgui_render_window(ImGuiViewport* viewport, void* windowParamsAsVoid);
    // Resize a imgui window
    void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size);
    // Present an imgui window, swapping the back with front buffer
    void imgui_swap_buffers(ImGuiViewport* viewport, void*);

    DEFINE_YES_NO_ENUM(ClearRenderTarget);

    // Am imgui window is a standalone window that's just used for imgui rendering
    // Rex uses the docking branch of imgui which supports imgui viewports getting dragged outside of the main window
    // This classes represents such a window that got created by dragging a viewport outside of the main window.
    class ImGuiWindow
    {
    public:
      ImGuiWindow(ImGuiViewport* viewport, const ImGuiDevice& creationInfo);

      // Render the imgui window
      void render(ClearRenderTarget clearRenderTarget, ImGuiWindowRenderParams* renderParams);

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
      RexImGuiViewport m_viewport;
    };
  }
}