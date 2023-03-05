#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/memory/memory_tracking.h"

#include <cstdlib>

namespace rex
{
  GlobalAllocator& global_allocator()
  {
    static GlobalAllocator alloc;
    return alloc;
  }
} // namespace rex