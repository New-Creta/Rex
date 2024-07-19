#include "rex_renderer_core/gfx/graphics_context.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/graphics_engine.h"
#include "rex_renderer_core/system/gpu_engine.h"

namespace rex
{
  namespace gfx
  {
    GraphicsContext::GraphicsContext(gfx::GraphicsEngine* owningEngine, GraphicsEngineType type)
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

    // Reset the context, freeing up any previously allocated commands
    void GraphicsContext::reset(ScopedPoolObject<gfx::PooledAllocator>&& alloc, ResourceStateTracker* resourceStateTracker, const ContextResetData& resetData)
    {
      REX_ASSERT_X(m_allocator.has_object() == false, "Overwriting the allocator of a gfx context is not allowed. You need to execute the commands of the context first");
      REX_ASSERT_X(alloc.has_object(), "Assigning a nullptr as allocator for a gfx context is not allowed.");
      m_allocator = rsl::move(alloc);
      m_resource_state_tracker = resourceStateTracker;

      platform_reset(m_allocator->underlying_alloc(), resetData);
      type_specific_reset(resetData);
    }
    // Execute the commands on the gpu.
    // A sync info is returned so the user can use it to sync with other contexts
    ScopedPoolObject<SyncInfo> GraphicsContext::execute_on_gpu(WaitForFinish waitForFinish)
    {
      // If we've already executed this context, we don't need to execute it again
      if (has_executed())
      {
        return {};
      }

      // Flush all render states back to the parent resource state tracker
      flush_render_states();

      // Execute the context and return the command allocator back to the pool
      ScopedPoolObject<SyncInfo> sync_info = m_owning_engine->execute_context(this, waitForFinish);
      m_allocator->reset_fence(sync_info->fence_val());
      m_allocator.return_to_pool();

      return sync_info;
    }
    // Flush all render states set in this command lists and notify the parent
    void GraphicsContext::flush_render_states()
    {
      m_resource_state_tracker->update_parent();
      m_resource_state_tracker->clear();
    }
    // Stall the gpu from executing this context until the commands the sync info represents has completed
    void GraphicsContext::stall(SyncInfo& syncInfo)
    {
      m_owning_engine->stall(syncInfo);
    }
    // Return if this context has executed on the gpu already
    bool GraphicsContext::has_executed() const
    {
      return !m_allocator.has_object();
    }
  
    // Return the type of this context
    GraphicsContextType GraphicsContext::type() const
    {
      return m_type;
    }

    // track a transition of a resource
    ResourceStateTransition GraphicsContext::track_resource_transition(Resource* buffer, ResourceState state)
    {
      return m_resource_state_tracker->track_resource_transition(buffer, state);
    }
    // Return the owning gpu engine of this context
    gfx::GraphicsEngine* GraphicsContext::owning_engine()
    {
      return m_owning_engine;
    }
    // Reset the engine speicifc context
    void GraphicsContext::type_specific_reset(const ContextResetData& resetData)
    {
      // Nothing to implement
    }
  }
}