#pragma once

#include "rex_renderer_core/gfx/command_allocator_pool.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/rhi/command_type.h"

#include "rex_engine/engine/types.h"

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
      {

      }

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

    private:
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      gfx::CommandAllocatorPool m_command_allocator_pool;
    };

    template <typename Context>
    class GraphicsEngine : public BaseGraphicsEngine
    {
    public:
      using context_type = Context;

      GraphicsEngine()
        : BaseGraphicsEngine(command_type_for_context())
        //: m_command_queue(rhi::create_command_queue(command_type_for_context()))
        //, m_command_allocator_pool()
        , m_idle_contexts()
        , m_active_contexts()
      {}

      // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
      ScopedPoolObject<context_type> new_context()
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
          context_type* ctx = transfer_context(idx, m_idle_contexts, m_active_contexts);
          ctx->reset(alloc);
          ScopedPoolObject<context_type> pooled_ctx(ctx, [this](context_type* ctx) { discard_context(ctx); });
          return pooled_ctx;
        }

        // Otherwise create a new one and add it to the active list
        return create_new_active_ctx(alloc);
      }
      // Returns a context object back to the engine
      void discard_context(context_type* context)
      {
        auto it = rsl::find_if(m_active_contexts.cbegin(), m_active_contexts.cend(), [context](const rsl::unique_ptr<context_type>& ctx) { return ctx.get() == context; });
        REX_ASSERT_X(it != m_active_contexts.cend(), "Trying to return context to engine that doesn't own it.");
        s32 idx = rsl::distance(m_active_contexts.cbegin(), it);
        transfer_context(idx, m_active_contexts, m_idle_contexts);
      }

      

    private:
      rhi::CommandType command_type_for_context()
      {
        if constexpr (rsl::is_same_v<context_type, rhi::RenderContext>)
        {
          return rhi::CommandType::Render;
        }

        if constexpr (rsl::is_same_v<context_type, rhi::CopyContext>)
        {
          return rhi::CommandType::Copy;
        }

        if constexpr (rsl::is_same_v<context_type, rhi::ComputeContext>)
        {
          return rhi::CommandType::Compute;
        }

        REX_ASSERT("Unknown command type for context of type {}", rsl::type_id<context_type>().name());
        return invalid_obj<rhi::CommandType>();
      }

      // Transfers a context object from one vector to the other
      context_type* transfer_context(s32 srcIdx, rsl::vector<rsl::unique_ptr<context_type>>& src, rsl::vector<rsl::unique_ptr<context_type>>& dst)
      {
        rsl::unique_ptr<context_type> ctx = rsl::move(src[srcIdx]);
        src.pop_back();
        dst.emplace_back(rsl::move(ctx));
        return dst.back().get();
      }

      // Creates a new context and adds it to the active list
      ScopedPoolObject<context_type> create_new_active_ctx(rhi::CommandAllocator* alloc)
      {
        rsl::unique_ptr<rhi::CommandList> cmdlist = rhi::create_commandlist();
        m_active_contexts.emplace_back(rsl::make_unique<context_type>(rsl::move(cmdlist), alloc));
        context_type* ctx = m_active_contexts.back().get();
        return ScopedPoolObject<context_type>(ctx, [this](context_type* ctx) { discard_context(ctx); });
      }

    private:
      //rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      //gfx::CommandAllocatorPool m_command_allocator_pool;
      rsl::vector<rsl::unique_ptr<context_type>> m_idle_contexts;
      rsl::vector<rsl::unique_ptr<context_type>> m_active_contexts;

    };

    // These classes will likely have graphics api specific implementations
  }
}