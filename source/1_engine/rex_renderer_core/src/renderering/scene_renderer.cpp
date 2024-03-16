#include "rex_renderer_core/rendering/scene_renderer.h"

#include "rex_renderer_core/rendering/scene.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    void SceneRenderer::render(Scene* scene, f32 width, f32 height)
    {
      scene->render(this, width, height);
    }

  } // namespace renderer
} // namespace rex