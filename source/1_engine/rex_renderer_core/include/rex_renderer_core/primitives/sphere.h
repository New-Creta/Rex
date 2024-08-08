#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/resources/static_mesh.h"

#include <glm/gtc/constants.hpp>

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
  {
    // Create a single sphere
    StaticMesh create_sphere(f32 radius, u16 sliceCount, u16 stackCount);

    // Create a single geo sphere
    // A geo sphere is a sphere but with all the triangles having the same surface area.
    StaticMesh create_geo_sphere(f32 radius, u32 numSubdivisions);
  } // namespace mesh_factory
  }
} // namespace rex