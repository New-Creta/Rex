#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/core/texture_format.h"

namespace rex
{
  namespace gfx
  {
    class CommandQueue;

    struct ImGuiDevice
    {
      CommandQueue* command_queue; // This is needed to create swapchains on imgui spawned windows
      s32 max_num_frames_in_flight;     // This is needed to initialize the swapchain on imgui spawned windows
      TextureFormat rtv_format; // This is needed to initialize the swapchain on imgui spawned windows
    };

    // Initialize the imgui device so imgui can use it to create its own windows
    void init_imgui_device(const ImGuiDevice& imguiDevice);
    // Retrieve the imgui device to initialize an imgui window
    const ImGuiDevice& imgui_device();
  }
}