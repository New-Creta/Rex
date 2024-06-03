#pragma once

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/rhi/command_type.h"
#include "rex_engine/pooling/scoped_pool_object.h"

namespace rex
{
  namespace gfx
  {
    class BaseGraphicsEngine;
    class GraphicsEngine;
  }

  namespace rhi
  {
    class CommandAllocator;
    class Buffer;

    // Structure used to syncronise multiple engine with each other
    // eg: If the compute engine needs to wait for the copy engine to finish
    // you somehow need to signal this to the compute engine
    // the sync info is the structure you need to do exactly that.
    // A sync info object is provided with every context.
    // It holds all the information another context (of the same or different context)
    // needs to wait for the first mentioned context to have executed on the gpu.
    class SyncInfo
    {
    public:
      SyncInfo(u64 fenceValue, void* fenceObject)
        : m_fence_value()
        , m_fence_object()
      {
        reset(fenceValue, fenceObject);
      }

      u64 fence_val() const
      {
        return m_fence_value;
      }
      void* fence_object()
      {
        return m_fence_object;
      }

      void reset(u64 fenceValue, void* fenceObject)
      {
        m_fence_value = fenceValue;
        m_fence_object = fenceObject;
      }

    private:
      u64 m_fence_value; // The fence value on the gpu after the owning context has executed
      void* m_fence_object; // the graphics API specific fence object
    };

    class GraphicsContext
    {
    public:
      GraphicsContext(gfx::GraphicsEngine* owningEngine, CommandType type);
      virtual ~GraphicsContext();

      void reset(CommandAllocator* alloc);
      void release_allocator(u64 fenceValue);
      ScopedPoolObject<SyncInfo> execute_on_gpu();
      void flush_render_states();
      ResourceStateTransition track_resource_transition(Buffer* buffer, ResourceState state);

      CommandType type() const;
      void stall(SyncInfo& syncInfo);

    protected:
      bool has_executed() const;
      gfx::BaseGraphicsEngine* owning_engine();

    private:
      rhi::CommandAllocator* m_allocator;
      gfx::BaseGraphicsEngine* m_owning_engine;
      ResourceStateTracker m_resource_state_tracker;
      CommandType m_type;
    };
  }
}