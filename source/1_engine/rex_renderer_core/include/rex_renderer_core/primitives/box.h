#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_std/array.h"

#include "rex_renderer_core/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // create a simple box mesh. 
      StaticMesh create_box(f32 width, f32 height, f32 depth);
    } // namespace mesh_factory
  }
} // namespace rex