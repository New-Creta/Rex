#include "rex_engine/memory/win/win_mem_stats.h"

// NOLINTBEGIN(llvm-include-order)
// clang-format off
#include <Windows.h>
#include <Psapi.h>
// clang-format on
// NOLINTEND(llvm-include-order)

namespace rex
{
  namespace win
  {
    MemoryStats query_memory_stats()
    {
      // retrieve the memory status from Windows
      MEMORYSTATUSEX memory_status_ex {};
      memory_status_ex.dwLength = sizeof(memory_status_ex);
      ::GlobalMemoryStatusEx(&memory_status_ex);

      // retrieve the processor memory info
      PROCESS_MEMORY_COUNTERS_EX process_mem_counters {};
      GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&process_mem_counters), sizeof(process_mem_counters)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      // retrieve the system info
      SYSTEM_INFO sys_info {};
      GetSystemInfo(&sys_info);

      // combine the information all together
      MemoryStats stats {};
      stats.total_physical_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullTotalPhys));
      stats.avail_physical_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullAvailPhys));
      stats.total_virtual_mem  = rsl::memory_size(static_cast<card64>(memory_status_ex.ullAvailVirtual));
      stats.page_size          = rsl::memory_size(sys_info.dwPageSize);
      stats.used_physical_mem  = rsl::memory_size(static_cast<card64>(process_mem_counters.WorkingSetSize));
      stats.used_virtual_mem   = rsl::memory_size(static_cast<card64>(process_mem_counters.PagefileUsage));
      stats.num_page_faults    = static_cast<card32>(process_mem_counters.PageFaultCount);
      return stats;
    }
  } // namespace win
} // namespace rex