#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_std/array.h"

namespace rex
{
  namespace mesh_factory
  {
    // create a simple box mesh. 
    MeshData16 create_box(f32 width, f32 height, f32 depth);
  } // namespace mesh_factory
} // namespace rex