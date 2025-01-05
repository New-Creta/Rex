#pragma once

#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_engine/gfx/resources/static_mesh.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      //  <------------------- cellsPerLine ------------------->
      // +-----------------+-----------------+-----------------+ 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // +-----------------+-----------------+-----------------+ 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | ^
      // +-----------------+-----------------+-----------------+ lineWidth
      // |                 |                 |                 | v
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // |                 |                 |                 | 
      // +-----------------+-----------------+-----------------+ 
      //                    <-- cellSize -->      
      // 
      // Create a single grid mesh                                                         
      StaticMesh create_grid(s16 cellSize, s32 cellsPerLine, f32 lineWidth);
    } // namespace mesh_factory
  }
} // namespace rex