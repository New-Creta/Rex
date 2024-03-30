#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_std/array.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_box(f32 width, f32 height, f32 depth, u32 subdivisions);
  } // namespace mesh_factory
} // namespace rex