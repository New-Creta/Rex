#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/memory/memory_tracking.h"

#include <cstdlib>

namespace rex
{
  GlobalAllocator& global_allocator()
  {
#ifdef REX_ENABLE_MEM_TRACKING
    static UntrackedAllocator untracked_alloc {};
    static GlobalAllocator alloc(untracked_alloc);
#else
    static GlobalAllocator alloc;
#endif
    return alloc;
  }

  GlobalDebugAllocator& global_debug_allocator()
  {
    // global debug allocations don't need to get tracked
    static UntrackedAllocator untracked_alloc {};
    static GlobalDebugAllocator alloc(untracked_alloc);

    return alloc;
  }
} // namespace rex