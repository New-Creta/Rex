#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{
  struct MemoryStats
  {
    // total physical memory
    rsl::memory_size total_physical_mem {};

    // available physical memory
    rsl::memory_size avail_physical_mem {};

    // total virtual memory available
    rsl::memory_size total_virtual_mem {};

    // the page size of the current system, aka how much memory memory is paged to disk
    rsl::memory_size page_size {};

    // the amount of memory that's currently used by physical memory
    rsl::memory_size used_physical_mem {};

    // the amount of memory that's paged out to disk
    rsl::memory_size used_virtual_mem {};

    // amount of page faults that have occurred
    // Note: this doesn't differentiate between hard and soft page faults
    // so don't assume that a high page fault count is bad
    card32 num_page_faults {};
  };
} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/memory/win_memory_stats.h"
#endif