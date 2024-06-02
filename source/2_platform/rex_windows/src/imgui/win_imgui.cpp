#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "imgui/platform/win/imgui_impl_win32.h"

namespace rex
{
  bool imgui_platform_init(void* platformWindowHandle)
  {
    ImGui_ImplWin32_Init(platformWindowHandle);

    // Maybe these can be done in the ImGui renderer
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    //platform_io.Renderer_CreateWindow = create_window_callback;   
    //platform_io.Renderer_DestroyWindow = destroy_window_callback;
    //platform_io.Renderer_SetWindowSize = set_window_size_callback;
    //platform_io.Renderer_RenderWindow = render_window_callback;
    //platform_io.Renderer_SwapBuffers = swap_buffers_callback;

    return true;
  }
  void imgui_platform_new_frame()
  {
    ImGui_ImplWin32_NewFrame();
  }
}