#include "rex_engine/gfx/imgui/imgui_renderer.h"

#include "imgui/platform/win/imgui_impl_win32.h"

#include "rex_engine/gfx/imgui/imgui_window.h"
#include "rex_engine/platform/win/imgui/win_imgui.h"

#include <Windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rex
{
  namespace gfx
  {
    bool imgui_platform_init(void* platformWindowHandle)
    {
      ImGui_ImplWin32_Init(platformWindowHandle);

      // Maybe these can be done in the ImGui renderer
      ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
      platform_io.Renderer_CreateWindow = rex::gfx::imgui_create_window;
      platform_io.Renderer_DestroyWindow = rex::gfx::imgui_destroy_window;
      platform_io.Renderer_SetWindowSize = rex::gfx::imgui_set_window_size;
      platform_io.Renderer_RenderWindow = rex::gfx::imgui_render_window;
      platform_io.Renderer_SwapBuffers = rex::gfx::imgui_swap_buffers;

      return true;
    }
    void imgui_platform_shutdown()
    {
      ImGui_ImplWin32_Shutdown();
    }

    void imgui_platform_new_frame()
    {
      ImGui_ImplWin32_NewFrame();
    }

  }

  bool imgui_main_window_update(win::Hwnd hwnd, card32 msg, win::WParam wparam, win::LParam lparam)
  {
    return ImGui_ImplWin32_WndProcHandler((HWND)hwnd, msg, wparam, lparam);
  }
}