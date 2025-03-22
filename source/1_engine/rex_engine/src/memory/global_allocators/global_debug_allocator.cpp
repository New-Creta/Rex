#include "rex_engine/memory/global_allocators/global_debug_allocator.h"

#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
  void* GlobalDebugAllocator::allocate(const s32 count)
  {
    return m_alloc.allocate(count);
  }
  void GlobalDebugAllocator::deallocate(void* const ptr, s32 /*count*/)
  {
    return m_alloc.deallocate(ptr);
  }

  s32 GlobalDebugAllocator::max_size() const
  {
    return (rsl::numeric_limits<s32>::max)();
  }

} // namespace rex