#include "rex_engine/memory/untracked_allocator.h"

#include <cstdlib>

namespace rex
{
  UntrackedAllocator::pointer UntrackedAllocator::allocate(rsl::memory_size size) // NOLINT(readability-convert-member-functions-to-static)
  {
    return allocate(size.size_in_bytes());
  }
  UntrackedAllocator::pointer UntrackedAllocator::allocate(card64 size) // NOLINT(readability-convert-member-functions-to-static)
  {
    return malloc(size);                                                // NOLINT(cppcoreguidelines-no-malloc)
  }

  void UntrackedAllocator::deallocate(pointer ptr, rsl::memory_size size) // NOLINT(readability-convert-member-functions-to-static)
  {
    deallocate(ptr, size.size_in_bytes());
  }
  void UntrackedAllocator::deallocate(pointer ptr, card64 /*size*/) // NOLINT(readability-convert-member-functions-to-static)
  {
    free(ptr);                                                      // NOLINT(cppcoreguidelines-no-malloc)
  }
} // namespace rex