#include "rex_engine/gfx/system/copy_context.h"

#include "rex_engine/gfx/system/copy_engine.h"

#include "rex_engine/gfx/system/view_heap.h"
#include "rex_engine/gfx/system/gpu_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Copy)
    {}

    const ResourceView* CopyContext::copy_texture_views_to_shaders(const rsl::vector<const ResourceView*>& views)
    {
      return copy_views(m_shader_visible_srv_heap, views);
    }
    const ResourceView* CopyContext::copy_sampler_views_to_shaders(const rsl::vector<const ResourceView*>& views)
    {
      return copy_views(m_shader_visible_sampler_heap, views);
    }
    const ResourceView* CopyContext::copy_views(ViewHeapType heapType, const rsl::vector<const ResourceView*>& views)
    {
      const ResourceView* gpuViews = gpu_engine()->try_get_gpu_views(views);
      if (gpuViews != nullptr)
      {
        return gpuViews;
      }

      switch (heapType)
      {
      case rex::gfx::ViewHeapType::Texture2D:       return copy_texture_views_to_shaders(views);
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

    const ResourceView* CopyContext::copy_views(ViewHeap* dstHeap, const rsl::vector<const ResourceView*>& views)
    {
      rsl::unique_ptr<ResourceView> gpuView = dstHeap->copy_views(views);
      return gpu_engine()->notify_views_on_gpu(views, rsl::move(gpuView));
    }

  }
}