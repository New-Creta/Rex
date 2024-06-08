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
  namespace rhi
  {
    class RenderTarget;
  }

  namespace renderer
  {
    DEFINE_YES_NO_ENUM(ClearRenderTarget);

    // Am imgui window is a standalone window that's just used for imgui rendering
    // Rex uses the docking branch of imgui which supports imgui viewports getting dragged outside of the main window
    // This classes represents such a window that got created by dragging a viewport outside of the main window.
    class ImGuiWindow
    {
    public:
      ImGuiWindow(ImGuiViewport* viewport, const ImGuiDevice& creationInfo);

      void render(ClearRenderTarget clearRenderTarget);

      void wait_for_pending_operations();
      void resize_buffers(s32 width, s32 height);
      void present();
      void yield_thread_until_in_sync_with_gpu();

    private:
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
      rsl::unique_ptr<rhi::ClearStateResource> m_clear_state;
      RexImGuiViewport m_viewport;
    };
  }
}