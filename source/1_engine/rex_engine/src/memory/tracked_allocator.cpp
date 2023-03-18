#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/memory/tracked_allocator.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std/assert.h"
#include "rex_std/memory.h"

namespace rex
{
  MemoryHeader* header_for_block(void* ptr)
  {
    // get the memory header for the allocator ptr
    rsl::byte* mem_block = static_cast<rsl::byte*>(ptr);
    mem_block -= sizeof(MemoryHeader*);
    MemoryHeader* header = *reinterpret_cast<MemoryHeader**>(mem_block); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return header;
  }

} // namespace rex