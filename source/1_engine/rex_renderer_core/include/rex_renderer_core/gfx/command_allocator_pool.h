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
    // When it's returned to the pool it's fence value is checked, making sure it's higher
    // than when originally allocated.
    class PooledAllocator
    {
    public:
      PooledAllocator(u64 fenceValue, rsl::unique_ptr<rhi::CommandAllocator> alloc);

      // Reset the fence value to a new value
      void reset_fence(u64 fenceValue);
      // Return the fence value
      u64 fence_value() const;

      // Easy access to underlying allocator object
      rhi::CommandAllocator* underlying_alloc();
      const rhi::CommandAllocator* underlying_alloc() const;

      rhi::CommandAllocator* operator->();
      const rhi::CommandAllocator* operator->() const;

    private:
      u64 m_fence_value; // The fence value required to be achieved in order to use the allocator
      rsl::unique_ptr<rhi::CommandAllocator> m_allocator;
    };
    // The pool holding all command allocators
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