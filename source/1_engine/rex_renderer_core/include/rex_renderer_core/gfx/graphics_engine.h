#pragma once

#include "rex_renderer_core/gfx/command_allocator_pool.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/rhi/command_type.h"
#include "rex_renderer_core/rhi/graphics_context.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    // A graphics engine is an engine responsible for their own respective field of the graphics pipeline.
    // Each graphics engine owns the command queue used for their part of the graphics pipeline.
    // They also hold a list of active and idle contexts which are used to queue up gpu commands
    // Lastly they also hold a command allocator pool which is used to allocate new command allocators from
    // and pool them up when they're not used.
    class GraphicsEngine
    {
    public:
      GraphicsEngine(rhi::CommandType type);
      virtual ~GraphicsEngine() = default;

      // Executes the context and returns the fence value that'll be set when all commands are executed
      ScopedPoolObject<rhi::SyncInfo> execute_context(rhi::GraphicsContext* context);
      
      // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
      ScopedPoolObject<rhi::GraphicsContext> new_context();
      // Returns a context object back to the engine
      void discard_context(rhi::GraphicsContext* context);

      // Fully initialize the engine, allocating all required resources etc
      virtual void init() = 0;

      // Flush all commands on the gpu and halt the current thread untill all commands are executed
      void flush();

      // LEFT TO CLEANUP
      // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

      u64 last_completed_fence() const;
      void release_allocator(u64 fenceValue, rhi::CommandAllocator* allocator);
      rhi::CommandAllocator* request_allocator();
      void stall(rhi::SyncInfo& sync_info);
      rhi::CommandQueue* command_queue();
      rhi::CommandType type() const;

      virtual void new_frame();

      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    protected:
      // Allocates a new API specific graphics context
      virtual rsl::unique_ptr<rhi::GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) = 0;

    private:
      // Creates a new context and adds it to the active list
      ScopedPoolObject<rhi::GraphicsContext> create_new_active_ctx(rhi::CommandAllocator* alloc);
      // Check if we have any idle contexts
      bool has_idle_contexts() const;
      // Pop the last idle context from the queue
      rhi::GraphicsContext* pop_idle_context(rhi::CommandAllocator* alloc);
      // Create a new context object using the idle contexts list
      ScopedPoolObject<rhi::GraphicsContext> create_from_idle_context(rhi::CommandAllocator* alloc);

    private:
      //rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      //gfx::CommandAllocatorPool m_command_allocator_pool;
      rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>> m_idle_contexts;
      rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>> m_active_contexts;
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      gfx::CommandAllocatorPool m_command_allocator_pool;

    };

    // These classes will likely have graphics api specific implementations
  }
}