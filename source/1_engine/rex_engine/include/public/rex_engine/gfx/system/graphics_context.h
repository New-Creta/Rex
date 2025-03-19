#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/system/command_allocator.h"
#include "rex_engine/gfx/system/resource_state_tracker.h"
#include "rex_engine/gfx/core/graphics_engine_type.h"
#include "rex_engine/engine/object_with_destruction_callback.h"
#include "rex_engine/gfx/system/command_allocator_pool.h"
#include "rex_engine/gfx/system/command_queue.h"
#include "rex_engine/gfx/system/sync_info.h"

#include "rex_std/bonus/utility.h"


// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECT WITH DESTRUCTION CALLBACK

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
    class PooledAllocator;
    class RenderTarget;
    class GpuEngine;
  }

  namespace gfx
  {
    class CommandAllocator;
    class Resource;
    class ViewHeap;
    struct ContextResetData;

    // A graphics context acts as the base class for the interface into queueing gpu commands
    class GraphicsContext
    {
    public:
      GraphicsContext(gfx::GraphicsEngine* owningEngine, GraphicsContextType type);
      virtual ~GraphicsContext();

      // Reset the context, freeing up any previously allocated commands
      // After this call, the context is in the same state as it was just constructed
      void reset(ObjectWithDestructionCallback<gfx::PooledAllocator>&& alloc, ResourceStateTracker* resourceStateTracker, const ContextResetData& resetData);
      // Execute the commands on the gpu.
      // A sync info is returned so the user can use it to sync with other contexts
      ObjectWithDestructionCallback<SyncInfo> execute_on_gpu(WaitForFinish waitForFinish = WaitForFinish::no);
      // Flush all render states set in this command lists and notify the parent tracker
      void flush_render_states();
      // Stall the gpu from executing this context until the commands the sync info represents has completed
      void stall(SyncInfo& syncInfo);
      // Return if this context has executed on the gpu already
      bool has_executed() const;

      // Return the type of this context
      GraphicsContextType type() const;


      // Profiling events
      virtual void begin_profile_event(rsl::string_view eventName) = 0;
      virtual void end_profile_event() = 0;

    protected:
      // track a transition of a resource
      ResourceStateTransition track_resource_transition(Resource* buffer, ResourceState state);
      // Return the owning gpu engine of this context
      gfx::GraphicsEngine* owning_engine();
      // Reset the engine speicifc context
      virtual void type_specific_reset(const ContextResetData& resetData);
      // Reset the platform specific context
      virtual void platform_reset(CommandAllocator* alloc, const ContextResetData& resetData) = 0;

      GpuEngine* gpu_engine();

    private:
      ObjectWithDestructionCallback<gfx::PooledAllocator> m_allocator;  // The allocator to use for allocating gpu commands
      gfx::GraphicsEngine* m_owning_engine;                // The owning engine of this context
      ResourceStateTracker* m_resource_state_tracker;      // The tracker for resource states
      GraphicsContextType m_type;                          // The type of this context
    };
  }
}