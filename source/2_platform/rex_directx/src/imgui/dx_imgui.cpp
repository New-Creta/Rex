#include "rex_directx/rendering/dx_imgui_window.h"
#include "rex_renderer_core/imgui/imgui_window.h"
#include "rex_renderer_core/imgui/imgui_resources.h"
#include "rex_engine/memory/global_allocator.h"

#include "rex_renderer_core/imgui/imgui_device.h"

namespace rex
{
  void imgui_create_window(ImGuiViewport* viewport)
  {
    void* mem = rex::global_debug_allocator().allocate<renderer::ImGuiWindow>();
    renderer::ImGuiWindow* imgui_window = new (mem)(renderer::ImGuiWindow)(viewport, imgui_device());
    viewport->RendererUserData = imgui_window;
  }
  void imgui_destroy_window(ImGuiViewport* viewport)
  {
    if (renderer::ImGuiWindow* imgui_window = (renderer::ImGuiWindow*)viewport->RendererUserData)
    {
      imgui_window->wait_for_pending_operations();
      rex::global_debug_allocator().destroy(imgui_window);
      rex::global_debug_allocator().deallocate(imgui_window);
    }
    viewport->RendererUserData = nullptr;
  }
  void imgui_render_window(ImGuiViewport* viewport, void*)
  {
    renderer::ImGuiWindow* imgui_window = (renderer::ImGuiWindow*)viewport->RendererUserData;
    const bool clear_render_target = !(viewport->Flags & ImGuiViewportFlags_NoRendererClear);

    imgui_window->render(clear_render_target);
  }
  void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size)
  {
    renderer::ImGuiWindow* imgui_window = (renderer::ImGuiWindow*)viewport->RendererUserData;
    imgui_window->wait_for_pending_operations();
    imgui_window->resize_buffers(size.x, size.y);
  }
  void imgui_swap_buffers(ImGuiViewport* viewport, void*)
  {
    renderer::ImGuiWindow* imgui_window = (renderer::ImGuiWindow*)viewport->RendererUserData;

    imgui_window->present();
    imgui_window->yield_thread_until_in_sync_with_gpu();
  }
}