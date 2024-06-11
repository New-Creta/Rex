#pragma once

#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/pooling/growing_pool.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  namespace gfx
  {
    // A structure combining an idle allocator and the fence value that's required
    // to have been reached to free up the allocator
    struct PooledAllocator
    {
      PooledAllocator(u64 fenceValue, rsl::unique_ptr<rhi::CommandAllocator> alloc)
        : fence_value(fenceValue)
        , allocator(rsl::move(alloc))
      {}

      u64 fence_value;
      rsl::unique_ptr<rhi::CommandAllocator> allocator;
    };
    class CommandAllocatorPool
    {
    public:
      CommandAllocatorPool(rhi::CommandType type);

      // Request a new allocator from the pool, create a new one if one isn't found
      ScopedPoolObject<PooledAllocator> request_allocator(u64 fenceValue);

    private:
      GrowingPool<PooledAllocator> m_pool;
      rhi::CommandType m_type;
    };
  }
}