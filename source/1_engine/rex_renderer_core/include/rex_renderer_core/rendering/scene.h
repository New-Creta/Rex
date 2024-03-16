#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace renderer
  {
    class SceneRenderer;

    class Scene
    {
    public:
      using RenderItems       = rsl::vector<RenderItem>;
      using RenderItemIt      = RenderItems::iterator;
      using ConstRenderItemIt = RenderItems::const_iterator;

      void add_render_item(RenderItem&& item);
      void clear_render_items();

      virtual void render(SceneRenderer* renderer);
      u32 render_item_count() const;

    protected:
      void update_object_constant_buffers(const rex::renderer::ResourceSlot& currObjectCommittedResource);
      void render_items(SceneRenderer* renderer, const rsl::vector<ResourceSlot>& contantBuffers) const;

    private:
      RenderItems m_render_items;
    };
  } // namespace renderer
} // namespace rex