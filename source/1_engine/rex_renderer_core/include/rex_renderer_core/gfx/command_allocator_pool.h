#pragma once

#include "rex_renderer_core/gfx/command_allocator_pool.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  namespace gfx
  {
    // The command allocator pool is a pool holding all allocators for a given command/engine type
    // Users can request a new allocator from it, a new one gets created if no idle allocators are available
    class CommandAllocatorPool
    {
      // A structure combining an idle allocator and the fence value that's required
      // to have been reached to free up the allocator
      struct PooledAllocator
      {
        u64 fence_value; // the fence value that got set for commands allocated by this allocator
        rsl::unique_ptr<rhi::CommandAllocator> allocator; // the allocator itself
      };

    public:
      CommandAllocatorPool(rhi::CommandType type);

      // Request a new allocator from the pool, create a new one if one isn't found
      rhi::CommandAllocator* request_allocator(u64 fenceValue);
      // Return an allocator back to the pool
      void discard_allocator(rhi::CommandAllocator* cmdAlloc, u64 fenceValue);

    private:
      // Find a free allocator in the pool and return its index
      s32 find_free_allocator(u64 fenceValue);
      // Return the free allocator found at the index
      rhi::CommandAllocator* use_existing_allocator(s32 idx);
      // Create a new allocator and add it to the active allocators
      rhi::CommandAllocator* create_new_active_alloc();

    private:
      rsl::vector<PooledAllocator> m_idle_allocators;   // Holds all allocators currently not in use
      rsl::vector<PooledAllocator> m_active_allocators; // Holds all allocators currently in use
      rhi::CommandType m_type; // The command/engine type of the allocators
    };

  }
}