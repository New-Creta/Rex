#include "rex_renderer_core/imgui/imgui_resources.h"

namespace rex
{
  ImGuiRenderState g_imgui_render_state;
  void imgui_init_resources(const ImGuiResources& resources)
  {
    g_imgui_render_state.root_signature = resources.root_signature;
    g_imgui_render_state.pso = resources.pso;
    g_imgui_render_state.constant_buffer = resources.cb;
    g_imgui_render_state.primitive_topology = renderer::PrimitiveTopology::TriangleList;
    g_imgui_render_state.blend_factor = { 0.0f, 0.0f, 0.0f, 0.0f };
  }


  const ImGuiRenderState& imgui_renderstate()
  {
    return g_imgui_render_state;
  }
}