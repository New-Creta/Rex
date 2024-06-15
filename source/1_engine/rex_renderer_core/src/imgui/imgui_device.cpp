#include "rex_renderer_core/imgui/imgui_device.h"

namespace rex
{
  ImGuiDevice g_imgui_device;

  // Initialize the imgui device so imgui can use it to create its own windows
  void init_imgui_device(const ImGuiDevice& imguiDevice)
  {
    g_imgui_device = imguiDevice;
  }

  // Retrieve the imgui device to initialize an imgui window
  const ImGuiDevice& imgui_device()
  {
    return g_imgui_device;
  }
}