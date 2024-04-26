#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

namespace rex
{
  namespace mesh_factory
  {
    // Create a single quad
    MeshData16 create_quad(f32 x, f32 y, f32 w, f32 h, f32 depth);
  } // namespace mesh_factory
} // namespace rex