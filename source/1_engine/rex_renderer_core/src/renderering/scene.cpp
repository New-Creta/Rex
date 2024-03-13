#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/rendering/scene_renderer.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    void Scene::add_render_item(RenderItem&& item)
    {
      m_render_items.push_back(rsl::move(item));
    }
    //-------------------------------------------------------------------------
    void Scene::clear_render_items()
    {
      m_render_items.clear();
    }

    void Scene::render(rex::renderer::SceneRenderer* renderer)
    {
      // Nothing to implement
    }

    //-------------------------------------------------------------------------
    u32 Scene::render_item_count() const
    {
      return m_render_items.size();
    }

    const Scene::RenderItems& Scene::render_items() const
    {
      return m_render_items;
    }

  } // namespace renderer
} // namespace rex