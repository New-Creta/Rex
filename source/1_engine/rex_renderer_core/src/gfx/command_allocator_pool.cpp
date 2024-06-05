#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  namespace gfx
  {
    CommandAllocatorPool::CommandAllocatorPool(rhi::CommandType type)
      : m_type(type)
    {}

    // Request a new allocator from the pool, create a new one if one isn't found
    rhi::CommandAllocator* CommandAllocatorPool::request_allocator(u64 fenceValue)
    {
      // First try to find if we have any idle allocators right now, if so, use one of those
      s32 free_alloc_idx = find_free_allocator(fenceValue);
      if (free_alloc_idx != -1)
      {
        return use_existing_allocator(free_alloc_idx);
      }

      // If not, create a new allocator and return that.
      return create_new_active_alloc();
    }
    // Return an allocator back to the pool
    void CommandAllocatorPool::discard_allocator(rhi::CommandAllocator* cmdAlloc, u64 fenceValue)
    {
      // Move the allocator from the active allocators and put it in the idle allocators
      auto it = rsl::find_if(m_active_allocators.begin(), m_active_allocators.end(), [cmdAlloc](const PooledAllocator& pooledAlloc) { return pooledAlloc.allocator.get() == cmdAlloc; });
      REX_ASSERT_X(it != m_active_allocators.end(), "Command allocator to discard is not found in active allocators. This is usually caused because the command allocator didn't come from this pool.");

      PooledAllocator pooled_alloc = rsl::move(*it);
      pooled_alloc.fence_value = fenceValue;
      m_active_allocators.erase(it);
      m_idle_allocators.emplace_back(rsl::move(pooled_alloc));
    }

    // Find a free allocator in the pool and return its index
    s32 CommandAllocatorPool::find_free_allocator(u64 fenceValue)
    {
      // Find an allocator in the idle allocators list that for sure has had its last command executed (this is determined by the fence value)
      auto it = rsl::find_if(m_idle_allocators.cbegin(), m_idle_allocators.cend(), [fenceValue](const PooledAllocator& pooledAlloc) { return pooledAlloc.fence_value <= fenceValue; });
      if (it != m_idle_allocators.cend())
      {
        return rsl::distance(m_idle_allocators.cbegin(), it);
      }

      return -1;
    }
    // Return the free allocator found at the index
    rhi::CommandAllocator* CommandAllocatorPool::use_existing_allocator(s32 idx)
    {
      // Move the allocator from the idle allocators and put it in the active allocators
      REX_ASSERT_X(idx != -1, "Invalid iterator used for allocator");
      auto it = m_idle_allocators.begin() + idx;
      m_active_allocators.emplace_back(rsl::move(*it));
      m_idle_allocators.erase(it);

      return m_active_allocators.back().allocator.get();
    }
    // Create a new allocator and add it to the active allocators
    rhi::CommandAllocator* CommandAllocatorPool::create_new_active_alloc()
    {
      // Simply create a new allocator from the rhi and put it in the pool
      u64 fence_value = 0;
      PooledAllocator& pooled_alloc = m_active_allocators.emplace_back(PooledAllocator{ fence_value, rhi::create_command_allocator(m_type) });
      return pooled_alloc.allocator.get();
    }

  }
}