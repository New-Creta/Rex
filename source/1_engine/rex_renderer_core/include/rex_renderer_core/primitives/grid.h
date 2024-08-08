#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // Create a single grid mesh
      StaticMesh create_grid(f32 width, f32 depth, s16 m, s16 n);
    } // namespace mesh_factory
  }
} // namespace rex