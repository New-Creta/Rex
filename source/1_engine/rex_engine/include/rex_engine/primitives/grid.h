#pragma once

#include "rex_engine/primitives/mesh_factory.h"

namespace rex
{
  namespace legacy
  {
    namespace mesh_factory
    {
      MeshData16 create_grid(f32 width, f32 depth, u16 m, u16 n);
    } // namespace mesh_factory
  }   // namespace legacy
} // namespace rex