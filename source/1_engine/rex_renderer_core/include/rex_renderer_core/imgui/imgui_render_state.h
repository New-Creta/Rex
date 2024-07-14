#pragma once

#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/materials/material.h"

#include "rex_std/array.h"

namespace rex
{
  namespace gfx
  {
    struct ImGuiRenderState
    {
      Material* material;             // The material to be used by all imgui rendering
    };
  }
}