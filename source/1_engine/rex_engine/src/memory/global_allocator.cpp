#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/memory/memory_tracking.h"

#include <cstdlib>

namespace rex
{
  GlobalAllocator& global_allocator()
  {
#ifdef REX_ENABLE_MEM_TRACKING
    static GlobalAllocator alloc{ UntrackedAllocator() };
#else
    static GlobalAllocator alloc;
#endif
    return alloc;
  }
} // namespace rex