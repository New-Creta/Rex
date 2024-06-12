#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/rhi/rhi.h"

#include "rex_engine/containers/vector_utils.h"

namespace rex
{
  namespace gfx
  {
    PooledAllocator::PooledAllocator(u64 fenceValue, rsl::unique_ptr<rhi::CommandAllocator> alloc)
      : m_fence_value(fenceValue)
      , m_allocator(rsl::move(alloc))
    {

    }

    void PooledAllocator::reset_fence(u64 fenceValue)
    {
      m_fence_value = fenceValue;
    }
    u64 PooledAllocator::fence_value() const
    {
      return m_fence_value;
    }

    rhi::CommandAllocator* PooledAllocator::underlying_alloc()
    {
      return m_allocator.get();
    }
    const rhi::CommandAllocator* PooledAllocator::underlying_alloc() const
    {
      return m_allocator.get();
    }
    rhi::CommandAllocator* PooledAllocator::operator->()
    {
      return m_allocator.get();
    }
    const rhi::CommandAllocator* PooledAllocator::operator->() const
    {
      return m_allocator.get();
    }


    CommandAllocatorPool::CommandAllocatorPool(rhi::GraphicsEngineType type)
      : m_type(type)
      , m_pool([this]() { return rsl::make_unique<PooledAllocator>(0, rhi::create_command_allocator(m_type)); })
    {}

    // Request a new allocator from the pool, create a new one if one isn't found
    ScopedPoolObject<PooledAllocator> CommandAllocatorPool::request_allocator(u64 fenceValue)
    {
      // Find an allocator who's fence value is equal or higher than the given fence value
      PooledAllocator* alloc = m_pool.request(
        [fenceValue](const rsl::unique_ptr<PooledAllocator>& alloc)
        {
          return alloc->fence_value() <= fenceValue;
        });

      // Return a scoped pool object with the knowledge how to add it back to the pool
      return ScopedPoolObject<PooledAllocator>(alloc,
        [this, fenceValue](PooledAllocator* alloc)
        {
          REX_ASSERT_X(fenceValue < alloc->fence_value(), "Make sure you increase an allocator's fence value before you return it back to the pool");
          m_pool.discard(alloc);
        });
    }
  }
}