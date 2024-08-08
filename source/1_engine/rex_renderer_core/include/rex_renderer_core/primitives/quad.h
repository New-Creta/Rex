#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

#include "rex_renderer_core/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // Create a single quad
      StaticMesh create_quad(f32 w, f32 h);
    } // namespace mesh_factory
  }
} // namespace rex