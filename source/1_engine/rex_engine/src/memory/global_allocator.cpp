#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/memory/memory_manager.h"

namespace rex
{
  void* GlobalAllocator::allocate(rsl::memory_size size)
  {
    return allocate(size.size_in_bytes());
  }
  void* GlobalAllocator::allocate(card64 size)
  {
    mem_manager().track_alloc(size);
    return malloc(size); // NOLINT(cppcoreguidelines-no-malloc)
  }

  void GlobalAllocator::deallocate(void* ptr, rsl::memory_size size)
  {
    deallocate(ptr, size.size_in_bytes());
  }
  void GlobalAllocator::deallocate(void* ptr, card64 size)
  {
    mem_manager().track_dealloc(size);
    free(ptr); // NOLINT(cppcoreguidelines-no-malloc)
  }

  GlobalAllocator& global_allocator()
  {
    static GlobalAllocator alloc;
    return alloc;
  }
} // namespace rex