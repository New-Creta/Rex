#include "rex_engine/memory/global_allocators/global_scratch_allocator.h"

#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
  void* GlobalScratchAllocator::allocate(const s32 count)
  {
    return mut_globals().allocators.scratch_allocator->allocate(count);
  }
  void GlobalScratchAllocator::deallocate(void* const ptr, s32 /*count*/)
  {
    return mut_globals().allocators.scratch_allocator->deallocate(ptr);
  }

  s32 GlobalScratchAllocator::max_size() const
  {
    return (rsl::numeric_limits<s32>::max)();
  }

}