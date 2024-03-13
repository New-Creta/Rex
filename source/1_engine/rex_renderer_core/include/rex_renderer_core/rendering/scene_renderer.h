#pragma once

namespace rex
{
  namespace renderer
  {
    class Scene;

    class SceneRenderer
    {
    public:
      void render(Scene* scene);
    };
  } // namespace renderer
} // namespace rex