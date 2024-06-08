#pragma once

#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/rhi/primitive_topology.h"

#include "rex_std/array.h"

namespace rex
{
  struct ImGuiRenderState
  {
    rhi::RootSignature* root_signature;
    rhi::PipelineState* pso;
    renderer::PrimitiveTopology primitive_topology;
    rsl::array<f32, 4> blend_factor;
  };
}