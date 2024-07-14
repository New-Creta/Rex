#pragma once

#include "imgui/imgui.h"

namespace rex
{
  namespace gfx
  {
    // Create a new imgui window object
    void imgui_create_window(ImGuiViewport* viewport);
    // Destroy an imgui window object
    void imgui_destroy_window(ImGuiViewport* viewport);
    // Render a given imgui window
    void imgui_render_window(ImGuiViewport* viewport, void* materialAsvoid);
    // Resize a imgui window
    void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size);
    // Present an imgui window, swapping the back with front buffer
    void imgui_swap_buffers(ImGuiViewport* viewport, void*);
  }
}