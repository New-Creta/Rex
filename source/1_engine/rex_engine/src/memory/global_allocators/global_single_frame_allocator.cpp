#include "rex_engine/memory/global_allocators/global_single_frame_allocator.h"

#include "rex_engine/engine/mutable_globals.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: HEAP AND ALLOCATORS

namespace rex
{
  void* SingleFrameAllocatorWrapper::allocate(const s32 count)
  {
    return mut_globals().allocators.single_frame_allocator->allocate(count);
  }
  void SingleFrameAllocatorWrapper::deallocate(void* const ptr, s32 /*count*/)
  {
    mut_globals().allocators.single_frame_allocator->deallocate(ptr);
  }

  s32 SingleFrameAllocatorWrapper::max_size() const
  {
    return (rsl::numeric_limits<s32>::max)();
  }

}