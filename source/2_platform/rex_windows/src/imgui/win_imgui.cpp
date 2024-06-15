#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "imgui/platform/win/imgui_impl_win32.h"

#include "rex_renderer_core/imgui/imgui_window.h"
#include "rex_windows/imgui/win_imgui.h"

#include <Windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rex
{
  bool imgui_platform_init(void* platformWindowHandle)
  {
    ImGui_ImplWin32_Init(platformWindowHandle);

    // Maybe these can be done in the ImGui renderer
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = imgui_create_window;
    platform_io.Renderer_DestroyWindow = imgui_destroy_window;
    platform_io.Renderer_SetWindowSize = imgui_set_window_size;
    platform_io.Renderer_RenderWindow = imgui_render_window;
    platform_io.Renderer_SwapBuffers = imgui_swap_buffers;

    return true;
  }
  void imgui_platform_new_frame()
  {
    ImGui_ImplWin32_NewFrame();
  }

  bool imgui_main_window_update(win::Hwnd hwnd, card32 msg, win::WParam wparam, win::LParam lparam)
  {
    return ImGui_ImplWin32_WndProcHandler((HWND)hwnd, msg, wparam, lparam);
  }
}