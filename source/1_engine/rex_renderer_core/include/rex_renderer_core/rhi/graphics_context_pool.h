#pragma once

#include "rex_engine/pooling/growing_pool.h"
#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_std/functional.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator;

    // The pool holding graphics contexts.
    // A pool is owned by the engine and is used to get a context from, for the given engine
    class GraphicsContextPool
    {
      using alloc_context_func = rsl::function<rsl::unique_ptr<GraphicsContext>(CommandAllocator*)>;

    public:
      GraphicsContextPool(rhi::GraphicsEngineType type, alloc_context_func&& allocContextFunc);

      // Request a new graphics context, which has for sure finished its commands.
      // If none is found, create a new one
      ScopedPoolObject<rhi::GraphicsContext> request(u64 lastCompletedFence, CommandAllocator* alloc);

    private:
      GrowingPool<GraphicsContext, rhi::CommandAllocator*> m_context_pool;
    };
  }
}