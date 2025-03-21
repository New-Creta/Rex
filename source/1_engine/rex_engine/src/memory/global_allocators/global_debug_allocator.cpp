#include "rex_engine/memory/global_allocators/global_debug_allocator.h"

#include "rex_engine/memory/allocators/untracked_allocator.h"

namespace rex
{
  UntrackedAllocator& untracked_allocator()
  {
    static UntrackedAllocator alloc {};
    return alloc;
  }
} // namespace rex