#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace gfx
  {
    // shader visibility is used to indicate which shader type has access to the specified resource
    enum class ShaderVisibility
    {
      Vertex = BIT(0), // only the vertex shader has access to this resource
      Pixel = BIT(1), // only the pixel shader has access to this resource
      Geometry = BIT(2), // only the geometry shader has access to this resource
      Compute = BIT(3), // only the compute shader has access to this resource
      Hull = BIT(4), // only the hull shader has access to this resource
      Domain = BIT(5), // only the domain shader has access to this resource
      Amplification = BIT(6), // only the amplification shader has access to this resource
      Mesh = BIT(7), // only the mesh shader has access to this resource

      All = Vertex | Pixel | Geometry | Compute | Hull | Domain | Amplification | Mesh // all shaders have access to this resource
    };
  }
}