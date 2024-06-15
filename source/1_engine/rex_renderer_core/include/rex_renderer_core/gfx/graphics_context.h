#pragma once

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/gfx/graphics_engine_type.h"
#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_renderer_core/system/command_allocator_pool.h"
#include "rex_renderer_core/system/sync_info.h"

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
    class PooledAllocator;
  }

  namespace gfx
  {
    class CommandAllocator;
    class Resource;
    class DescriptorHeap;

    // A graphics context acts as the base class for the interface into queueing gpu commands
    class GraphicsContext
    {
    public:
      GraphicsContext(gfx::GraphicsEngine* owningEngine, GraphicsEngineType type);
      virtual ~GraphicsContext();

      // Reset the context, freeing up any previously allocated commands
      void reset(ScopedPoolObject<gfx::PooledAllocator>&& alloc, ResourceStateTracker* resourceStateTracker, DescriptorHeap* descHeap);
      // Execute the commands on the gpu.
      // A sync info is returned so the user can use it to sync with other contexts
      ScopedPoolObject<SyncInfo> execute_on_gpu();
      // Flush all render states set in this command lists and notify the parent
      void flush_render_states();
      // Stall the gpu from executing this context until the commands the sync info represents has completed
      void stall(SyncInfo& syncInfo);
      // Return if this context has executed on the gpu already
      bool has_executed() const;

      // Return the type of this context
      GraphicsEngineType type() const;

    protected:
      // track a transition of a resource
      ResourceStateTransition track_resource_transition(Resource* buffer, ResourceState state);
      // Return the owning gpu engine of this context
      gfx::GraphicsEngine* owning_engine();
      // Reset the platform specific context
      virtual void platform_reset(CommandAllocator* alloc, DescriptorHeap* descHeap) = 0;

    private:
      ScopedPoolObject<gfx::PooledAllocator> m_allocator; // The allocator to use for allocating gpu commands
      gfx::GraphicsEngine* m_owning_engine;               // The owning engine of this context
      ResourceStateTracker* m_resource_state_tracker;      // The tracker for resource states
      GraphicsEngineType m_type;                          // The type of this context
    };
  }
}