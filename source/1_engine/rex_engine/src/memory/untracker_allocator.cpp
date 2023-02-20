#include "rex_engine/memory/untracker_allocator.h"

namespace rex
{
  void* UntrackedAllocator::allocate(rsl::memory_size size)
  {
    return allocate(size.size_in_bytes());
  }
  void* UntrackedAllocator::allocate(card64 size)
  {
    return malloc(size); // NOLINT(cppcoreguidelines-no-malloc)
  }

  void UntrackedAllocator::deallocate(void* ptr, rsl::memory_size size)
  {
    deallocate(ptr, size.size_in_bytes());
  }
  void UntrackedAllocator::deallocate(void* ptr, card64 /*size*/)
  {
    free(ptr); // NOLINT(cppcoreguidelines-no-malloc)
  }
}