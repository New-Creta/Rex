#include "rex_renderer_core/gfx/graphics_engine.h"

#include "rex_engine/containers/vector_utils.h"

namespace rex
{
  namespace gfx
  {
    GraphicsEngine::GraphicsEngine(rhi::CommandType type)
      : m_command_queue(rhi::create_command_queue(type))
      , m_command_allocator_pool(type)
      , m_idle_contexts()
      , m_active_contexts()
    {}

    // Executes the context and returns the fence value that'll be set when all commands are executed
    ScopedPoolObject<rhi::SyncInfo> GraphicsEngine::execute_context(rhi::GraphicsContext* context)
    {
      return m_command_queue->execute_context(context);
    }
    
    // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
    ScopedPoolObject<rhi::GraphicsContext> GraphicsEngine::new_context()
    {
      // Find a command alloctor to be used for the context
      rhi::CommandAllocator* alloc = request_allocator();

      // Check if we don't have a context already available but idle, if so, reset it and return it
      if (has_idle_contexts())
      {
        return create_from_idle_context(alloc);
      }

      // Otherwise create a new one and add it to the active list
      return create_new_active_ctx(alloc);
    }
    // Returns a context object back to the engine
    void GraphicsEngine::discard_context(rhi::GraphicsContext* context)
    {
      // Make sure this context is actually coming from this engine
      auto it = rsl::find_if(m_active_contexts.cbegin(), m_active_contexts.cend(), [context](const rsl::unique_ptr<rhi::GraphicsContext>& ctx) { return ctx.get() == context; });
      REX_ASSERT_X(it != m_active_contexts.cend(), "Trying to return context to engine that doesn't own it.");
      
      s32 idx = rsl::distance(m_active_contexts.cbegin(), it);
      rex::transfer_object(idx, m_active_contexts, m_idle_contexts);
    }

    // Create a new context and add it directly to the active contexts list
    ScopedPoolObject<rhi::GraphicsContext> GraphicsEngine::create_new_active_ctx(rhi::CommandAllocator* alloc)
    {
      m_active_contexts.emplace_back(allocate_new_context(alloc));
      rhi::GraphicsContext* ctx = m_active_contexts.back().get();
      return ScopedPoolObject<rhi::GraphicsContext>(ctx, [this](rhi::GraphicsContext* ctx)
        {
          if (!ctx->has_executed())
          {
            ctx->execute_on_gpu();
          }
          discard_context(ctx);
        });
    }
    // Check if we have any idle contexts
    bool GraphicsEngine::has_idle_contexts() const
    {
      return m_idle_contexts.size() > 0;
    }
    // Pop the last idle context from the queue
    rhi::GraphicsContext* GraphicsEngine::pop_idle_context(rhi::CommandAllocator* alloc)
    {
      s32 idx = m_idle_contexts.size() - 1;
      rhi::GraphicsContext* ctx = rex::transfer_object(idx, m_idle_contexts, m_active_contexts).get();
      ctx->reset(alloc);

      return ctx;
    }
    // Create a new context object using the idle contexts list
    ScopedPoolObject<rhi::GraphicsContext> GraphicsEngine::create_from_idle_context(rhi::CommandAllocator* alloc)
    {
      rhi::GraphicsContext* ctx = pop_idle_context(alloc);

      ScopedPoolObject<rhi::GraphicsContext> pooled_ctx(ctx,
        [this](rhi::GraphicsContext* ctx)
        {
          if (!ctx->has_executed())
          {
            ctx->execute_on_gpu();
          }
          discard_context(ctx);
        });

      return pooled_ctx;
    }

    void GraphicsEngine::flush()
    {
      m_command_queue->flush();
    }

    void GraphicsEngine::release_allocator(u64 fenceValue, rhi::CommandAllocator* allocator)
    {
      m_command_allocator_pool.discard_allocator(allocator, fenceValue);
    }
    rhi::CommandAllocator* GraphicsEngine::request_allocator()
    {
      u64 last_completed_fence = m_command_queue->last_completed_fence();
      return m_command_allocator_pool.request_allocator(last_completed_fence);
    }
    void GraphicsEngine::stall(rhi::SyncInfo& syncInfo)
    {
      m_command_queue->gpu_wait(syncInfo);
    }
    rhi::CommandQueue* GraphicsEngine::command_queue()
    {
      return m_command_queue.get();
    }
    rhi::CommandType GraphicsEngine::type() const
    {
      return m_command_queue->type();
    }
  }
}