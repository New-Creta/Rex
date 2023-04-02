#pragma once

#include "rex_engine/types.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  struct MemoryStats
  {
    rsl::memory_size total_physical_mem; // total physical memory
    rsl::memory_size avail_physical_mem; // available physical memory
    rsl::memory_size total_virtual_mem; // total virtual memory available
    rsl::memory_size page_size; // the page size of the current system, aka how much memory memory is paged to disk
    rsl::memory_size used_physical_mem; // the amount of memory that's currently used by physical memory
    rsl::memory_size used_virtual_mem; // the amount of memory that's paged out to disk
  };
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/memory/win/win_mem_stats.h"
#endif