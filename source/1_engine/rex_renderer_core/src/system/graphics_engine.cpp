#include "rex_renderer_core/system/graphics_engine.h"

#include "rex_engine/containers/vector_utils.h"

namespace rex
{
  namespace gfx
  {
    GraphicsEngine::GraphicsEngine(GraphicsEngineType type, ResourceStateTracker* globalResourceStateTracker)
      : m_command_queue(rhi::create_command_queue(type))
      , m_command_allocator_pool(type)
      , m_context_pool([this](CommandAllocator* alloc) { return allocate_new_context(alloc); })
      , m_resource_state_tracker(globalResourceStateTracker)
    {}
    GraphicsEngine::~GraphicsEngine()
    {
      // Flush all pending operations on the gpu
      // We cannot destroy until all queued operations are executed
      flush();
    }

    // Executes the context and returns the fence value that'll be set when all commands are executed
    ScopedPoolObject<SyncInfo> GraphicsEngine::execute_context(GraphicsContext* context, WaitForFinish waitForFinish)
    {
      context->end_profile_event();
      return m_command_queue->execute_context(context, waitForFinish);
    }
    
    // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
    ScopedPoolObject<GraphicsContext> GraphicsEngine::new_context(DescriptorHeap* descHeap, rsl::string_view eventName)
    {
      // Find a command alloctor to be used for the context
      ScopedPoolObject<PooledAllocator> alloc = request_allocator();
      ScopedPoolObject<GraphicsContext> ctx = m_context_pool.request(alloc->underlying_alloc());

      // Always reset a context, making it ready to be used by a user
      ctx->begin_profile_event(eventName);
      ctx->reset(rsl::move(alloc), &m_resource_state_tracker, descHeap);
      return ctx;
    }

    // Halt gpu commands from being executed until the sync info object is triggered
    void GraphicsEngine::stall(SyncInfo& syncInfo)
    {
      m_command_queue->gpu_wait(syncInfo);
    }

    // Return the command queue owned by the graphics engine, this is needed for the swapchain
    CommandQueue* GraphicsEngine::command_queue()
    {
      return m_command_queue.get();
    }
    // Return the command type of the engine
    GraphicsEngineType GraphicsEngine::type() const
    {
      return m_command_queue->type();
    }
    // Return the resource tracker of this engine
    ResourceStateTracker* GraphicsEngine::resource_state_tracker()
    {
      return &m_resource_state_tracker;
    }

    // Flush all commands on the gpu and halt the current thread untill all commands are executed
    void GraphicsEngine::flush()
    {
      m_command_queue->flush();
    }

    // Request a new allocator from the command allocator pool
    ScopedPoolObject<PooledAllocator> GraphicsEngine::request_allocator()
    {
      u64 last_completed_fence = m_command_queue->last_completed_fence();
      return m_command_allocator_pool.request_allocator(last_completed_fence);
    }
  }
}