#include "rex_renderer_core/gfx/copy_context.h"

#include "rex_renderer_core/system/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Copy)
    {}

    rsl::unique_ptr<ResourceView> CopyContext::copy_texture_views_to_shaders(const rsl::vector<ResourceView*>& views)
    {
      return copy_views(m_shader_visible_srv_heap, views);
    }
    rsl::unique_ptr<ResourceView> CopyContext::copy_sampler_views_to_shaders(const rsl::vector<ResourceView*>& views)
    {
      return copy_views(m_shader_visible_sampler_heap, views);
    }

    void CopyContext::type_specific_reset(const ContextResetData& resetData)
    {
      m_shader_visible_srv_heap = resetData.shader_visible_srv_desc_heap;
      m_shader_visible_sampler_heap = resetData.shader_visible_sampler_desc_heap;
    }
  }
}