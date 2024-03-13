#include "rex_renderer_core/rendering/scene_renderer.h"

#include "rex_renderer_core/rendering/scene.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    void SceneRenderer::render(Scene* scene)
    {
      scene->render(this);
    }

  } // namespace renderer
} // namespace rex