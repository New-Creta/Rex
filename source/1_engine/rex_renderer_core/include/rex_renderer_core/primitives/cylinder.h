#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

#include <glm/gtc/constants.hpp>

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // Create a simple cylinder mesh
      MeshData16 create_cylinder(f32 bottomRadius, f32 topRadius, f32 height, u16 sliceCount, u16 stackCount);
    } // namespace mesh_factory
  }
} // namespace rex