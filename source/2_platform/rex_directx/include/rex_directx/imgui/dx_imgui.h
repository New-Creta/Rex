#pragma once

#include "rex_renderer_core/imgui/imgui_window.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandQueue;
  }

  struct ImGuiDevice
  {
    rhi::CommandQueue* command_queue;
    s32 max_num_frames_in_flight;
    DXGI_FORMAT rtv_format;
  };

  void init_dx_imgui_device(const ImGuiDevice& imguiDevice);
}