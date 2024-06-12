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
    rhi::RootSignature* root_signature;             // Root signature used for the imgui rendering, this will later become a material
    rhi::PipelineState* pso;                        // The pipeline state used for imgui rendering
    renderer::PrimitiveTopology primitive_topology; // The primitive topology used for imgui rendering
    rsl::array<f32, 4> blend_factor;                // The blend factor used for imgui rendering
  };
}