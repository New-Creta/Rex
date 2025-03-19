#include "rex_engine/platform/win/memory/win_memory_stats.h"

// NOLINTBEGIN(llvm-include-order)
// clang-format off
#include <Windows.h>
#include <Psapi.h>

#include "rex_engine/memory/memory_stats.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/memory/memory_size.h"
// clang-format on
// NOLINTEND(llvm-include-order)

namespace rex
{
  SystemMemoryStats query_system_mem_stats()
  {
    // retrieve the memory status from Windows
    MEMORYSTATUSEX memory_status_ex{};
    memory_status_ex.dwLength = sizeof(memory_status_ex);
    ::GlobalMemoryStatusEx(&memory_status_ex);

    // retrieve the system info
    SYSTEM_INFO sys_info{};
    GetSystemInfo(&sys_info);

    SystemMemoryStats stats{};
    stats.total_physical_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullTotalPhys));
    stats.avail_physical_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullAvailPhys));
    stats.total_virtual_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullTotalVirtual));
    stats.avail_virtual_mem = rsl::memory_size(static_cast<card64>(memory_status_ex.ullAvailVirtual));
    stats.page_size = rsl::memory_size(sys_info.dwPageSize);

    return stats;
  }

  ProcessMemoryStats query_process_mem_stats()
  {
    // As of Windows 10 22H2 with September 2023 cumulative update or Windows 11 22H2 with September 2023 cumulative update
    // A beter version is available called PROCESS_MEMORY_COUNTERS_EX2 
    // However, we cannot guarantee that everyone has this version installed so we'll use the old one for now
    // see: https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2?devlangs=cpp&f1url=%3FappId%3DDev16IDEF1%26l%3DEN-US%26k%3Dk(PSAPI%252FPROCESS_MEMORY_COUNTERS_EX2)%3Bk(PROCESS_MEMORY_COUNTERS_EX2)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue

    // retrieve the processor memory info
    PROCESS_MEMORY_COUNTERS_EX process_mem_counters {};
    GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&process_mem_counters), sizeof(process_mem_counters)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    // combine the information all together
    ProcessMemoryStats stats {};
    stats.used_physical_mem  = rsl::memory_size(static_cast<card64>(process_mem_counters.WorkingSetSize));
    stats.peak_physical_mem  = rsl::memory_size(static_cast<card64>(process_mem_counters.PeakWorkingSetSize));
    stats.used_virtual_mem   = rsl::memory_size(static_cast<card64>(process_mem_counters.PagefileUsage));
    stats.peak_virtual_mem   = rsl::memory_size(static_cast<card64>(process_mem_counters.PeakPagefileUsage));
    stats.num_page_faults    = static_cast<card32>(process_mem_counters.PageFaultCount);
    return stats;
  }
} // namespace rex