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

      All = Vertex | Pixel // all shaders have access to this resource
    };
  }
}