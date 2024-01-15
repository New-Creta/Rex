#pragma once

#include "rex_engine/app/core_application.h"
#include "rex_engine/diagnostics/stacktrace.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/memory_header.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_std/bonus/memory/memory_size.h"

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

namespace rex
{
  template <typename Allocator>
  class TrackedAllocator
  {
  public:
    using size_type = typename Allocator::size_type;
    using pointer   = typename Allocator::pointer;

    explicit TrackedAllocator(Allocator& alloc)
        : m_allocator(rsl::addressof(alloc))
    {
    }

    REX_NO_DISCARD pointer allocate(rsl::memory_size size)
    {
      return allocate(size.size_in_bytes());
    }
    REX_NO_DISCARD pointer allocate(size_type size)
    {
      // calculate the number of bytes are needed for the allocation
      size_type num_mem_needed = size;
      num_mem_needed += sizeof(MemoryHeader*);

      // allocate the memory with enough extra memory to fit the memory header pointer
      pointer ptr = m_allocator->allocate(num_mem_needed);

      rex::MemoryHeader* dbg_header_ptr = mem_tracker().track_alloc(ptr, num_mem_needed);

      // put the memory header pointer in front of the data blob we're going to return
      rsl::memcpy(ptr, &dbg_header_ptr, sizeof(dbg_header_ptr)); // NOLINT(bugprone-sizeof-expression)

      // get the right address to return from the function
      rsl::byte* mem_block = static_cast<rsl::byte*>(jump_forward(ptr, sizeof(MemoryHeader*)));

      return mem_block;
    }

    void deallocate(pointer ptr, rsl::memory_size size)
    {
      deallocate(ptr, size.size_in_bytes());
    }
    void deallocate(pointer ptr, size_type size)
    {
      if(ptr == nullptr)
      {
        return;
      }

      rsl::byte* mem_block = static_cast<rsl::byte*>(jump_backward(ptr, sizeof(MemoryHeader*)));
      MemoryHeader* header = *reinterpret_cast<MemoryHeader**>(mem_block); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      mem_tracker().track_dealloc(header);
      m_allocator->deallocate(mem_block, size);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      m_allocator->construct(p, rsl::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T* ptr)
    {
      m_allocator->destroy(ptr);
    }

    bool operator==(const TrackedAllocator& rhs) const
    {
      return *m_allocator == *rhs.m_allocator;
    }
    bool operator!=(const TrackedAllocator& rhs) const
    {
      return !(*this == rhs);
    }

  private:
    Allocator* m_allocator;
  };
} // namespace rex