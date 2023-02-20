#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/memory/memory_manager.h"
#include "rex_engine/memory/memory_tracking.h"

namespace rex
{
  GlobalAllocator& global_allocator()
  {
    static GlobalAllocator alloc;
    return alloc;
  }
}