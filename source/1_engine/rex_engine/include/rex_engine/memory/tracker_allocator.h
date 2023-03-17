#pragma once

#include "rex_engine/types.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_std/memory.h"

namespace rex
{
  template <typename Allocator>
  class TrackedAllocator
  {
  public:
    TrackedAllocator(Allocator alloc)
      : m_allocator(alloc)
    {}

    REX_NO_DISCARD void* allocate(rsl::memory_size size)
    {
      return allocate(size.size_in_bytes());
    }
    REX_NO_DISCARD void* allocate(card64 size)
    {
      // Memory tracking strategy
      // We allocate "size" + sizeof(MemoryHeader)
      // The memory header holds the tracking info of the ptr
      // we need to make sure the memory is aligned properly as well
      
      // first make sure we allocate enough memory
      card64 num_mem_needed = size;
      num_mem_needed += sizeof(MemoryHeader);

      void* ptr = m_allocator.allocate(num_mem_needed);

      // initialize the memory header
      const MemoryTag tag = mem_tracker().current_tag();
      rex::MemoryHeader* header = new(ptr) MemoryHeader(tag, rsl::memory_size(num_mem_needed), frame_info().index());

      // get the right address to return from the function
      rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
      mem_block += sizeof(MemoryHeader);

      // track the allocation
      mem_tracker().track_alloc(mem_block, header);

      return mem_block;
    }

    void deallocate(void* ptr, rsl::memory_size size)
    {
      deallocate(ptr, size.size_in_bytes());
    }
    void deallocate(void* ptr, card64 size)
    {
      if (ptr == nullptr)
      {
        return;
      }

      rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
      mem_block -= sizeof(MemoryHeader);

      MemoryHeader* header = reinterpret_cast<MemoryHeader*>(mem_block); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      mem_tracker().track_dealloc(ptr, header);
      m_allocator.deallocate(mem_block, size);
    }

  private:
    Allocator m_allocator;
  };
} // namespace rex