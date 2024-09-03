#pragma once

#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_std/array.h"

#include "rex_engine/gfx/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // create a simple box mesh. 
      StaticMesh create_box(f32 width, f32 height, f32 depth);

      // create a simple cube mesh. 
      StaticMesh create_cube(f32 size);
    } // namespace mesh_factory
  }
} // namespace rex