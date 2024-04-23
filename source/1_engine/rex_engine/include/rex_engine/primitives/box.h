#pragma once

#include "rex_engine/primitives/mesh_factory.h"
#include "rex_std/array.h"

namespace rex
{
  namespace legacy
  {
    namespace mesh_factory
    {
      MeshData16 create_box(f32 width, f32 height, f32 depth, u32 subdivisions);
    } // namespace mesh_factory
  }   // namespace legacy
} // namespace rex