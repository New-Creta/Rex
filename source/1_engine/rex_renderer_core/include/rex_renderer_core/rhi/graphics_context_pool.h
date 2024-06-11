#pragma once

#include "rex_engine/pooling/growing_pool.h"
#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_std/functional.h"

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
  }

  namespace rhi
  {
    class CommandAllocator;
    class GraphicsContextPool
    {
      using alloc_context_func = rsl::function<rsl::unique_ptr<GraphicsContext>(CommandAllocator* alloc)>;

    public:
      GraphicsContextPool(rhi::CommandType type, alloc_context_func&& allocContextFunc);
      ScopedPoolObject<rhi::GraphicsContext> request(u64 lastCompletedFence, CommandAllocator* alloc);

    private:
      GrowingPool<GraphicsContext, rhi::CommandAllocator*> m_context_pool;

    };
  }
}