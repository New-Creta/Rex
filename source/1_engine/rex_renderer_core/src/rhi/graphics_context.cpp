#include "rex_renderer_core/rhi/graphics_context.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/graphics_engine.h"

namespace rex
{
  namespace rhi
  {
    GraphicsContext::GraphicsContext(gfx::GraphicsEngine* owningEngine, CommandType type)
      : m_allocator()
      , m_owning_engine(owningEngine)
      , m_resource_state_tracker()
      , m_type(type)
    {

    }

    GraphicsContext::~GraphicsContext()
    {
      execute_on_gpu();
    }

    void GraphicsContext::reset(ScopedPoolObject<gfx::PooledAllocator>&& alloc)
    {
      REX_ASSERT_X(m_allocator.has_object() == false, "Overwriting the allocator of a gfx context is not allowed. You need to execute the commands of the context first");
      REX_ASSERT_X(alloc.has_object(), "Assigning a nullptr as allocator for a gfx context is not allowed.");
      m_allocator = rsl::move(alloc);

      platform_reset(alloc->underlying_alloc());
    }

    ScopedPoolObject<SyncInfo> GraphicsContext::execute_on_gpu()
    {
      if (has_executed())
      {
        return {};
      }

      flush_render_states();

      ScopedPoolObject<SyncInfo> sync_info = m_owning_engine->execute_context(this);
      m_allocator->reset_fence(sync_info->fence_val());
      m_allocator.return_to_pool();

      return sync_info;
    }

    void GraphicsContext::flush_render_states()
    {
      m_resource_state_tracker.update_parent();
      m_resource_state_tracker.clear();
    }

    CommandType GraphicsContext::type() const
    {
      return m_type;
    }

    void GraphicsContext::stall(SyncInfo& syncInfo)
    {
      m_owning_engine->stall(syncInfo);
    }

    bool GraphicsContext::has_executed() const
    {
      return !m_allocator.has_object();
    }

    gfx::GraphicsEngine* GraphicsContext::owning_engine()
    {
      return m_owning_engine;
    }

    ResourceStateTransition GraphicsContext::track_resource_transition(Resource* buffer, ResourceState state)
    {
      return m_resource_state_tracker.track_resource_transition(buffer, state);
    }
  }
}