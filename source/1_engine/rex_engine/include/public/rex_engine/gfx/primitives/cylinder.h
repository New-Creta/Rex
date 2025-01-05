#pragma once

#include "rex_engine/gfx/primitives/mesh_factory.h"

#include "rex_engine/gfx/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // Create a simple cylinder mesh
      StaticMesh create_cylinder(f32 bottomRadius, f32 topRadius, f32 height, u16 sliceCount, u16 stackCount);
    } // namespace mesh_factory
  }
} // namespace rex