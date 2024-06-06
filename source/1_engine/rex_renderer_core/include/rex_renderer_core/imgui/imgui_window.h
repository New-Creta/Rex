#pragma once

#include "imgui/imgui.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rhi/texture_format.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/system/swapchain.h"
#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/imgui/imgui_viewport.h"

namespace rex
{
  void imgui_create_window(ImGuiViewport* viewport);
  void imgui_destroy_window(ImGuiViewport* viewport);
  void imgui_render_window(ImGuiViewport* viewport, void*);
  void imgui_set_window_size(ImGuiViewport* viewport, ImVec2 size);
  void imgui_swap_buffers(ImGuiViewport* viewport, void*);
}