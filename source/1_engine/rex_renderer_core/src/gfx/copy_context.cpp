#include "rex_renderer_core/gfx/copy_context.h"

#include "rex_renderer_core/system/copy_engine.h"

#include "rex_renderer_core/system/view_heap.h"

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
    rsl::unique_ptr<ResourceView> CopyContext::copy_views(ViewHeapType heapType, const rsl::vector<ResourceView*>& views)
    {
      switch (heapType)
      {
      case rex::gfx::ViewHeapType::ResourceView:    return copy_texture_views_to_shaders(views);
      case rex::gfx::ViewHeapType::Sampler:         return copy_sampler_views_to_shaders(views);
      default: break;
      }

      return nullptr;
    }

    void CopyContext::type_specific_reset(const ContextResetData& resetData)
    {
      m_shader_visible_srv_heap = resetData.shader_visible_srv_desc_heap;
      m_shader_visible_sampler_heap = resetData.shader_visible_sampler_desc_heap;
    }

    rsl::unique_ptr<ResourceView> CopyContext::copy_views(ViewHeap* dstHeap, const rsl::vector<ResourceView*>& descriptors)
    {
      return dstHeap->copy_views(descriptors);
    }

  }
}