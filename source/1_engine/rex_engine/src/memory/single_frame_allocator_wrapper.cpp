#include "rex_engine/memory/single_frame_allocator_wrapper.h"

#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
  void* SingleFrameAllocatorWrapper::allocate(const s32 count)
  {
    return mut_globals().single_frame_allocator->allocate(count);
  }
  void SingleFrameAllocatorWrapper::deallocate(void* const ptr, s32 /*count*/)
  {
    mut_globals().single_frame_allocator->deallocate(ptr);
  }

  s32 SingleFrameAllocatorWrapper::max_size() const
  {
    return (rsl::numeric_limits<s32>::max)();
  }

}