#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace renderer
  {
    enum class ShaderVisibility
    {
      Vertex = BIT(0),
      Pixel = BIT(1),

      All = Vertex | Pixel
    };
  }
}