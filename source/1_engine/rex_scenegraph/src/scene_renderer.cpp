#include "rex_scenegraph/scene_renderer.h"
#include "rex_scenegraph/scene.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    SceneRenderer::SceneRenderer(const Scene* scene)
        : m_scene(scene)
    {
    }

  } // namespace renderer
} // namespace rex