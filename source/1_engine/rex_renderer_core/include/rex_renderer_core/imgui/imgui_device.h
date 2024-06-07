#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rhi/texture_format.h"

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
    renderer::TextureFormat rtv_format;
  };

  void init_imgui_device(const ImGuiDevice& imguiDevice);
  const ImGuiDevice& imgui_device();
}