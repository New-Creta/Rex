#pragma once

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/rhi/command_type.h"
#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_renderer_core/gfx/command_allocator_pool.h"
#include "rex_renderer_core/gfx/sync_info.h"

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
    struct PooledAllocator;
  }

  namespace rhi
  {
    class CommandAllocator;
    class Resource;

    class GraphicsContext
    {
    public:
      GraphicsContext(gfx::GraphicsEngine* owningEngine, CommandType type);
      virtual ~GraphicsContext();

      void reset(ScopedPoolObject<gfx::PooledAllocator>&& alloc);
      ScopedPoolObject<SyncInfo> execute_on_gpu();
      void flush_render_states();
      ResourceStateTransition track_resource_transition(Resource* buffer, ResourceState state);

      CommandType type() const;
      void stall(SyncInfo& syncInfo);
      bool has_executed() const;

    protected:
      gfx::GraphicsEngine* owning_engine();
      virtual void platform_reset(rhi::CommandAllocator* alloc) {};

    private:
      ScopedPoolObject<gfx::PooledAllocator> m_allocator;
      gfx::GraphicsEngine* m_owning_engine;
      ResourceStateTracker m_resource_state_tracker;
      CommandType m_type;
    };
  }
}