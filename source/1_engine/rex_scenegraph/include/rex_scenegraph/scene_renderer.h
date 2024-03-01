#pragma once

namespace rex
{
  namespace renderer
  {
    class Scene;

    class SceneRenderer
    {
    public:
      explicit SceneRenderer(const Scene* scene);

    private:
      const Scene* m_scene;
    };
  } // namespace renderer
} // namespace rex