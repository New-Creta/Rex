#pragma once

#include "rex_engine/pooling/growing_pool.h"
#include "rex_engine/gfx/system/graphics_context.h"
#include "rex_engine/gfx/system/command_allocator_pool.h"

#include "rex_std/functional.h"

namespace rex
{
  namespace gfx
  {
    class CommandAllocator;

    // The pool holding graphics contexts.
    // A pool is owned by the engine and is used to get a context from, for the given engine
    class GraphicsContextPool
    {
      using alloc_context_func = rsl::function<rsl::unique_ptr<GraphicsContext>(CommandAllocator*)>;

    public:
      GraphicsContextPool(alloc_context_func&& allocContextFunc);

      // Request a new graphics context, which has for sure finished its commands.
      // If none is found, create a new one
      ScopedPoolObject<GraphicsContext> request(CommandAllocator* alloc);

    private:
      GrowingPool<GraphicsContext, CommandAllocator*> m_context_pool;
    };
  }
}