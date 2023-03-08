#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/memory/tracker_allocator.h"
#include "rex_std/assert.h"
#include "rex_std/memory.h"

namespace rex
{
  void* TrackedAllocator::allocate(rsl::memory_size size)
  {
    return allocate(size.size_in_bytes());
  }
  void* TrackedAllocator::allocate(card64 size) // NOLINT(readability-convert-member-functions-to-static)
  {
    // Memory tracking strategy
    // We allocate "size" + sizeof(MemoryHeader)
    // The memory header holds the tracking info of the ptr
    // we need to make sure the memory is aligned properly as well

    // first make sure we allocate enough memory
    card64 num_mem_needed = size;
    num_mem_needed += sizeof(MemoryHeader);

    void* ptr = malloc(num_mem_needed); // NOLINT(cppcoreguidelines-no-malloc)

    // initialize the memory header
    const MemoryTag tag             = mem_tracker().current_tag();
    rex::MemoryHeader* header = new(ptr) MemoryHeader(tag, rsl::memory_size(num_mem_needed));

    // get the right address to return from the function
    rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
    mem_block += sizeof(MemoryHeader);

    // track the allocation
    mem_tracker().track_alloc(mem_block, header);

    return mem_block;
  }

  void TrackedAllocator::deallocate(void* ptr, rsl::memory_size size) 
  {
    deallocate(ptr, size.size_in_bytes());
  }
  void TrackedAllocator::deallocate(void* ptr, card64 /*size*/) // NOLINT(readability-convert-member-functions-to-static)
  {
    if(ptr == nullptr)
    {
      return;
    }

    rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
    mem_block -= sizeof(MemoryHeader);

    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(mem_block); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    mem_tracker().track_dealloc(ptr, header);
    free(header); // NOLINT(cppcoreguidelines-no-malloc)
  }
} // namespace rex