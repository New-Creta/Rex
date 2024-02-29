#include "rex_windows/crash_handler/win_crash_handler.h"

#include "rex_std/stacktrace.h"
#include "rex_std/mutex.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/threading/assert_mtx.h"

#include <Windows.h>

namespace rex
{
  namespace win
  {
    DEFINE_LOG_CATEGORY(CrashHandlingLog, rex::LogVerbosity::Log);

    AssertMtx& crash_assert_mtx()
    {
      static AssertMtx mtx;
      return mtx;
    }

    s32 report_crash(void* exceptionInfo)
    {
      rsl::unique_lock lock(crash_assert_mtx());

      LPEXCEPTION_POINTERS exception_info = static_cast<LPEXCEPTION_POINTERS>(exceptionInfo);

      DWORD exc_code = exception_info->ExceptionRecord->ExceptionCode;

      // This code is 7 frames after the actual crash, coming from a structured exception handling block
      const s32 frames_to_skip = 7;
      auto stack = rsl::stacktrace::current(frames_to_skip);

      // We cannot allocate heap memory as we have possibly run out of memory
      // Therefore we cannot use any logging and need to perform all operations on the stack
      REX_ERROR(CrashHandlingLog, "Error occurred!");
      for (const rsl::stacktrace_entry& line : stack)
      {
        REX_ERROR(CrashHandlingLog, line.description());
      }

      return EXCEPTION_EXECUTE_HANDLER;
    }
  }
}