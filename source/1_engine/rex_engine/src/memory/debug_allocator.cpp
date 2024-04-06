#include "rex_engine/memory/debug_allocator.h"

#include "rex_engine/memory/untracked_allocator.h"

namespace rex
{
  UntrackedAllocator& untracked_allocator()
  {
    static UntrackedAllocator alloc {};
    return alloc;
  }
} // namespace rex