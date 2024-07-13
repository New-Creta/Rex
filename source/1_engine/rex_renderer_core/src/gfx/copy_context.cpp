#include "rex_renderer_core/gfx/copy_context.h"

#include "rex_renderer_core/system/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Copy)
    {}

    void CopyContext::engine_reset(const ContextResetData& resetData)
    {
      m_global_srv_heap = resetData.global_srv_desc_heap;
      m_shader_visible_srv_heap = resetData.shader_visible_srv_desc_heap;
    }

    DescriptorHeap* CopyContext::global_srv_heap()
    {
      return m_global_srv_heap;
    }
    DescriptorHeap* CopyContext::shader_visible_srv_heap()
    {
      return m_shader_visible_srv_heap;
    }
  }
}