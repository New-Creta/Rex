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
      , m_context_pool(type, [this](rhi::CommandAllocator* alloc) { return allocate_new_context(alloc); })
    {}

    GraphicsEngine::~GraphicsEngine()
    {
      flush();
    }

    // Executes the context and returns the fence value that'll be set when all commands are executed
    ScopedPoolObject<rhi::SyncInfo> GraphicsEngine::execute_context(rhi::GraphicsContext* context)
    {
      return m_command_queue->execute_context(context);
    }
    
    // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
    ScopedPoolObject<rhi::GraphicsContext> GraphicsEngine::new_context()
    {
      // Find a command alloctor to be used for the context
      ScopedPoolObject<PooledAllocator> alloc = request_allocator();

      ScopedPoolObject<rhi::GraphicsContext> ctx = m_context_pool.request(m_command_queue->last_completed_fence(), alloc->underlying_alloc());
      ctx->reset(rsl::move(alloc));

      return ctx;
    }

    void GraphicsEngine::flush()
    {
      m_command_queue->flush();
    }

    ScopedPoolObject<PooledAllocator> GraphicsEngine::request_allocator()
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