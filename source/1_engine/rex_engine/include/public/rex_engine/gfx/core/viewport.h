#pragma once

#include "rex_engine/engine/types.h"

#include "glm/glm.hpp"

namespace rex
{
  namespace gfx
  {
    // Graphics api abstraction of the viewport
    struct Viewport
    {
      Viewport()
        : top_left()
        , size()
        , min_depth(0.0f)
        , max_depth(0.0f)
      {
      }

      Viewport(glm::vec2 topLeft, glm::vec2 size, f32 minDepth, f32 maxDepth)
        : top_left(topLeft)
        , size(size)
        , min_depth(minDepth)
        , max_depth(maxDepth)
      {
      }

      glm::vec2 top_left;
      glm::vec2 size;

      f32 min_depth;
      f32 max_depth;
    };
  }

} // namespace rex