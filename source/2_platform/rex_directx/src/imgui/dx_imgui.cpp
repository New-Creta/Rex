#include "rex_directx/imgui/dx_imgui_window.h"
#include "rex_renderer_core/imgui/imgui_window.h"
#include "rex_renderer_core/imgui/imgui_resources.h"
#include "rex_engine/memory/global_allocator.h"

#include "rex_renderer_core/imgui/imgui_device.h"

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
    void imgui_render_window(ImGuiViewport* viewport, void*)
    {
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
      const bool clear_render_target = !(viewport->Flags & ImGuiViewportFlags_NoRendererClear);

      imgui_window->render(clear_render_target);
    }
    // Resize a imgui window
    void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size)
    {
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
      imgui_window->wait_for_pending_operations();
      imgui_window->resize_buffers(size.x, size.y);
    }
    // Present an imgui window, swapping the back with front buffer
    void imgui_swap_buffers(ImGuiViewport* viewport, void*)
    {
      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;

      imgui_window->present();
      imgui_window->yield_thread_until_in_sync_with_gpu();
    }
  }
}