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
    // A graphics engine is an engine that drives a specific pipeline on the gpu (eg. copy, render or compute)
    class BaseGraphicsEngine
    {
    public:
      BaseGraphicsEngine(rhi::CommandType commandType)
        : m_command_queue(rhi::create_command_queue(commandType))
        , m_command_allocator_pool(commandType)
      {

      }
      virtual ~BaseGraphicsEngine() = default;

      // Executes the context and returns the fence value that'll be set when all commands are executed
      ScopedPoolObject<rhi::SyncInfo> execute_context(rhi::GraphicsContext* context)
      {
        return m_command_queue->execute_context(context);
      }

      u64 last_completed_fence() const
      {
        return m_command_queue->last_completed_fence();
      }

      void release_allocator(u64 fenceValue, rhi::CommandAllocator* allocator)
      {
        m_command_allocator_pool.discard_allocator(allocator, fenceValue);
      }

      rhi::CommandAllocator* request_allocator()
      {
        return m_command_allocator_pool.request_allocator(last_completed_fence());
      }

      void stall(rhi::SyncInfo& sync_info)
      {
        m_command_queue->gpu_wait(sync_info);
      }

      rhi::CommandQueue* command_queue()
      {
        return m_command_queue.get();
      }

      rhi::CommandType type() const
      {
        return m_command_queue->type();
      }

    private:
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      gfx::CommandAllocatorPool m_command_allocator_pool;
    };

    //template <typename Context>
    class GraphicsEngine : public BaseGraphicsEngine
    {
    public:
      //using context_type = Context;

      GraphicsEngine(rhi::CommandType type)
        : BaseGraphicsEngine(type)
        //: m_command_queue(rhi::create_command_queue(command_type_for_context()))
        //, m_command_allocator_pool()
        , m_idle_contexts()
        , m_active_contexts()
      {}

      // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
      template <typename Context>
      ScopedPoolObject<Context> new_context()
      {
        // Find a command alloctor to be used for the context
        // Allocators are added back to the ready queue in CPU time.
        // This means the last commandlist that used the allocator might not have finished executing yet.
        // Therefore we need to check the fence value of our command queue and request a command allocator
        // that's finished on the GPU.
        rhi::CommandAllocator* alloc = request_allocator();

        // Check if we don't have a context already available but idle, if so, reset it and return it
        if (!m_idle_contexts.empty())
        {
          s32 idx = m_idle_contexts.size() - 1;
          rhi::GraphicsContext* ctx = transfer_context(idx, m_idle_contexts, m_active_contexts);
          ctx->reset(alloc);
          Context* derived_context = static_cast<Context*>(ctx);
          ScopedPoolObject<Context> pooled_ctx(derived_context, 
            [this](Context* ctx) 
            { 
              if (!ctx->has_executed())
              {
                ctx->execute_on_gpu();
              }
              discard_context(ctx); 
            });
          return pooled_ctx;
        }

        // Otherwise create a new one and add it to the active list
        return create_new_active_ctx<Context>(alloc);
      }
      // Returns a context object back to the engine
      void discard_context(rhi::GraphicsContext* context)
      {
        auto it = rsl::find_if(m_active_contexts.cbegin(), m_active_contexts.cend(), [context](const rsl::unique_ptr<rhi::GraphicsContext>& ctx) { return ctx.get() == context; });
        REX_ASSERT_X(it != m_active_contexts.cend(), "Trying to return context to engine that doesn't own it.");
        s32 idx = rsl::distance(m_active_contexts.cbegin(), it);
        transfer_context(idx, m_active_contexts, m_idle_contexts);
      }

      virtual void post_init() = 0;

    protected:
      // Creates a new context and adds it to the active list
      virtual rsl::unique_ptr<rhi::GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) = 0;

    private:
      // Transfers a context object from one vector to the other
      rhi::GraphicsContext* transfer_context(s32 srcIdx, rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>>& src, rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>>& dst)
      {
        rsl::unique_ptr<rhi::GraphicsContext> ctx = rsl::move(src[srcIdx]);
        src.pop_back();
        dst.emplace_back(rsl::move(ctx));
        return dst.back().get();
      }

      // Creates a new context and adds it to the active list
      template <typename Context>
      ScopedPoolObject<Context> create_new_active_ctx(rhi::CommandAllocator* alloc)
      {
        m_active_contexts.emplace_back(allocate_new_context(alloc));
        rhi::GraphicsContext* ctx = m_active_contexts.back().get();
        Context* derived_context = static_cast<Context*>(ctx);
        return ScopedPoolObject<Context>(derived_context, [this](rhi::GraphicsContext* ctx) 
          { 
            if (!ctx->has_executed())
            {
              ctx->execute_on_gpu();
            }
            discard_context(ctx); 
          });
      }

    private:
      //rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      //gfx::CommandAllocatorPool m_command_allocator_pool;
      rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>> m_idle_contexts;
      rsl::vector<rsl::unique_ptr<rhi::GraphicsContext>> m_active_contexts;

    };

    // These classes will likely have graphics api specific implementations
  }
}