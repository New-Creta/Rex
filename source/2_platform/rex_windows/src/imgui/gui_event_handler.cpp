#include "rex_windows/imgui/gui_event_handler.h"

#include "platform/win/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows message handler for ImGui.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rex
{
  ImGUIEventHandler::ImGUIEventHandler() = default;
  ImGUIEventHandler::~ImGUIEventHandler() = default;

  win::LResult ImGUIEventHandler::wnd_proc_handler(win::Hwnd hWnd, u32 msg, win::WParam wParam, win::LParam lParam)
  {
    return ImGui_ImplWin32_WndProcHandler((HWND)hWnd, (UINT)msg, (WPARAM)wParam, (LPARAM)lParam);
  }
} // namespace rex