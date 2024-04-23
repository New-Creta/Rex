#include "rex_engine/platform/win/system/win_sys_info.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/memory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rex
{
  namespace sys_info
  {
    s32 number_physical_cores()
    {
      static s32 core_count = 0;

      // We only want to check this once
      if(core_count == 0)
      {
        // Get only physical cores
        DWORD buffer_size = 0;

        // Get the size of the buffer to hold processor information.
        WIN_CALL_IGNORE(GetLogicalProcessorInformation(nullptr, &buffer_size), ERROR_INSUFFICIENT_BUFFER);

        // Allocate the buffer to hold the processor info.
        rsl::unique_array<char> buffer                    = rsl::make_unique<char[]>(buffer_size); // NOLINT(modernize-avoid-c-arrays)
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION slpi_buffer = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.get()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        // Get the actual information.
        WIN_CALL(GetLogicalProcessorInformation(slpi_buffer, &buffer_size));

        // Count physical cores
        const int32 info_count = static_cast<int32>(buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        for(int32 idx = 0; idx < info_count; ++idx)
        {
          SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info = &slpi_buffer[idx];
          if(info->Relationship == RelationProcessorCore)
          {
            ++core_count;
          }
        }
      }

      return core_count;
    }

    s32 number_logical_processors()
    {
      static s32 core_count = 0;

      // We only want to check this once
      if(core_count == 0)
      {
        SYSTEM_INFO sys_info {};
        GetSystemInfo(&sys_info);
        core_count = static_cast<s32>(sys_info.dwNumberOfProcessors);
      }

      return core_count;
    }
  } // namespace sys_info
} // namespace rex