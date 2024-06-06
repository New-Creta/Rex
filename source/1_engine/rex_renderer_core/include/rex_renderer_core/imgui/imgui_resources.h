#pragma once

#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/imgui/imgui_render_state.h"

namespace rex
{
  struct ImGuiResources
  {
    rhi::RootSignature* root_signature;
    rhi::PipelineState* pso;
    rhi::ConstantBuffer* cb;
  };

  void imgui_init_resources(const ImGuiResources& resources);
  const ImGuiRenderState& imgui_renderstate();
}