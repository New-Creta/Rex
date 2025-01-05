#pragma once

#include "rex_engine/gfx/primitives/mesh_factory.h"

#include "rex_engine/gfx/resources/static_mesh.h"

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