#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    class Scene;

    class SceneRenderer
    {
    public:
      void render(Scene* scene, f32 width, f32 height);
    };
  } // namespace renderer
} // namespace rex