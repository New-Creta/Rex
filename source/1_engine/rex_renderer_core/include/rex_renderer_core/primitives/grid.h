#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_grid(f32 width, f32 depth, u16 m, u16 n);
  } // namespace mesh_factory
} // namespace rex