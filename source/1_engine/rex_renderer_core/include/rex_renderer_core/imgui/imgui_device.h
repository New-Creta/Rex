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
    rhi::CommandQueue* command_queue; // This is needed to create swapchains on imgui spawned windows
    s32 max_num_frames_in_flight;     // This is needed to initialize the swapchain on imgui spawned windows
    renderer::TextureFormat rtv_format; // This is needed to initialize the swapchain on imgui spawned windows
  };

  void init_imgui_device(const ImGuiDevice& imguiDevice);
  const ImGuiDevice& imgui_device();
}