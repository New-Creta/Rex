#pragma once

#include "rex_engine/primitives/mesh_factory.h"

#include <glm/gtc/constants.hpp>

namespace rex
{
  namespace legacy
  {
    namespace mesh_factory
    {
      MeshData16 create_cylinder(f32 bottomRadius, f32 topRadius, f32 height, u16 sliceCount, u16 stackCount);
    } // namespace mesh_factory
  }   // namespace legacy
} // namespace rex