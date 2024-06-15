#pragma once

#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/imgui/imgui_render_state.h"

namespace rex
{
  // Wrapper around imgui resources that are shared between all imgui viewports
  // This will become a material in the future
  struct ImGuiResources
  {
    rhi::RootSignature* root_signature;
    rhi::PipelineState* pso;
  };

  // Initialize the global imgui resources so they can be used by viewports
  void imgui_init_resources(const ImGuiResources& resources);
  // Retrieve the global imgui resources
  const ImGuiRenderState& imgui_renderstate();
}