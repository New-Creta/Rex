#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

namespace rex
{
  namespace mesh_factory
  {
    // Create a single grid mesh
    MeshData16 create_grid(f32 width, f32 depth, s16 m, s16 n);
  } // namespace mesh_factory
} // namespace rex