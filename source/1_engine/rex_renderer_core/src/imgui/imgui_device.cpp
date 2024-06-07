#include "rex_renderer_core/imgui/imgui_device.h"

namespace rex
{
  ImGuiDevice g_imgui_device;

  void init_imgui_device(const ImGuiDevice& imguiDevice)
  {
    g_imgui_device = imguiDevice;
  }

  const ImGuiDevice& imgui_device()
  {
    return g_imgui_device;
  }
}