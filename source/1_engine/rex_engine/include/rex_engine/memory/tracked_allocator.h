#pragma once

#include "rex_engine/types.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/debug_allocator.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_std/memory.h"

//
// Memory tracking strategy
// +-------------+------------------------------------------------------------------+
// |             |                                                                  |
// |  Header Ptr |                            DATA                                  |
// |             |                                                                  |
// +------+------+------------------------------------------------------------------+
//        |
//        |                 +-------------------------------------------------------+
//        |                 |                                                       |
//        +---------------->|              MEMORY HEADER IN DEBUG MEMORY            |
//                          |                                                       |
//                          +-------------------------------------------------------+
//
// Each pointer has a header that track information about the pointer
// To limit the size of the data returned, this memory header is allocated using debug memory.
// This allows us to have the allocated memory overhead to be only the size of a pointer
// While the memory header still exists elsewhere but isn't loaded in the cache when loading the pointer
// As these memory headers are quite big (due to stack string usage for callstacks) we don't want to 
// load all of this data when accessing a pointer and we force an indirection there.
// This also allows us to have future optimizations to have all memory headers allocated continuously
// so it can be queried easily by tools.
// Because the memory header sits in debug memory, we are sure this memory overhead won't exist in retail builds
// and will therefore result in crashes should it be accessed.
// 
// There's not much of a point to use this allocator as a stand alone as it only provides extra info regarding pointers
// but doesn't do anything with this info. A client is responsible for doing something with this extra info.
// 
// Eg. let's say you want to track memory leaks throughout your program, you'll have to track which memory stays allocated over time
// Therefore you can use the tracked allocator as a internal allocator, retrieve the MemoryHeader of the memory you just allocated
// and save this data to a file/send it to a tool, which can track which memory stays allocated and can therefore spot memory leaks.

namespace rex
{
  MemoryHeader* header_for_block(void* ptr);

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
      // calculate the number of bytes are needed for the allocation
      card64 num_mem_needed = size;
      num_mem_needed += sizeof(MemoryHeader*);

      // allocate the memory with enough extra memory to fit the memory header pointer
      void* ptr = m_allocator.allocate(num_mem_needed);

      // initialize the memory header
      const MemoryTag tag = mem_tracker().current_tag();
      const rsl::thread::id thread_id = rsl::this_thread::get_id();
      rex::GlobalDebugAllocator& dbg_alloc = rex::global_debug_allocator();
      rex::MemoryHeader* header_addr = static_cast<rex::MemoryHeader*>(dbg_alloc.allocate(sizeof(MemoryHeader)));
      rex::MemoryHeader* header_ptr = new(header_addr) MemoryHeader(tag, ptr, rsl::memory_size(num_mem_needed), thread_id, frame_info().index(), rsl::stacktrace::current());

      // put the memory header pointer in front of the data blob we're going to return
      rsl::memcpy(ptr, &header_ptr, sizeof(header_ptr));
      
      // get the right address to return from the function
      rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
      mem_block += sizeof(MemoryHeader*);

      // track the allocation
      mem_tracker().track_alloc(mem_block, header_ptr);

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

      MemoryHeader* header = header_for_block(ptr);
      mem_tracker().track_dealloc(ptr, header);

      rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
      mem_block -= sizeof(MemoryHeader*);

      global_debug_allocator().deallocate(header, size);
      m_allocator.deallocate(mem_block, size);
    }

  private:
    Allocator m_allocator;
  };
} // namespace rex