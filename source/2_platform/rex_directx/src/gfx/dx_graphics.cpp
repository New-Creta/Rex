#include "rex_renderer_core/gfx/graphics.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    GpuEngine::GpuEngine()
      : m_render_engine(rsl::make_unique<RenderEngine>())
      , m_compute_engine(rsl::make_unique<ComputeEngine>())
      , m_copy_engine(rsl::make_unique<CopyEngine>())
    {

    }


    // Get the command queue of the specified type and check if its internal fence
    // is higher or equal to the given value
    bool GpuEngine::is_fence_complete(rhi::CommandType cmdType, s32 fenceValue) const
    {
      REX_ASSERT("Function not yet implemented");
      return false;
    }
    // Get the command queue of the specified type and wait for its internal fence
    // to be higher or equal to the given value
    void GpuEngine::wait_for_fence(rhi::CommandType cmdType, s32 fenceValue)
    {
      REX_ASSERT("Function not yet implemented");

    }
    // Wait for all command queue to have finished executing their current queued commands
    void GpuEngine::wait_for_gpu()
    {
      REX_ASSERT("Function not yet implemented");

    }

    ScopedPoolObject<CopyContext> GpuEngine::new_copy_ctx()
    {
      return m_copy_engine->new_context();
    }

    ScopedPoolObject<rhi::RenderContext> GpuEngine::new_render_ctx()
    {
      return m_render_engine->new_context();
    }

    ScopedPoolObject<ComputeContext> GpuEngine::new_compute_ctx()
    {
      return m_compute_engine->new_context();
    }
  }
}