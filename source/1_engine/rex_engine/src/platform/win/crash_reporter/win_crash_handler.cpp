#include "rex_engine/platform/win/crash_reporter/win_crash_handler.h"

#include "rex_std/stacktrace.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"
#include "rex_std/bonus/platform.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/threading/assert_mtx.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"

#include <Windows.h>
#include <DbgHelp.h>

namespace rex
{
  namespace win
  {
    DEFINE_LOG_CATEGORY(CrashHandlingLog, rex::LogVerbosity::Info);

    DEFINE_YES_NO_ENUM(CreateFullDump);

    // The assertion mutex used by the crash handler
    AssertMtx& crash_assert_mtx()
    {
      static AssertMtx mtx;
      return mtx;
    }

    // Create a mini dump using Windows API
    void create_dump(rsl::string_view filepath, LPEXCEPTION_POINTERS exceptionInfo, CreateFullDump createFullDump = CreateFullDump::no)
    {
      // We need to create the file before we start creating the dump
      rsl::win::handle file_handle(CreateFileA(filepath.data(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));

      MINIDUMP_EXCEPTION_INFORMATION dump_exc_info{};

      // Initialize the exception information
      dump_exc_info.ThreadId = GetCurrentThreadId();
      dump_exc_info.ExceptionPointers = exceptionInfo;
      dump_exc_info.ClientPointers = true;

      MINIDUMP_TYPE minidump_type{};
 
      // Full dumps hold the entire state of the process including whatever's loaded in memory
      // Be careful when using these as they can fill up your disk quite quickly
      if (createFullDump)
      {
        minidump_type = static_cast<MINIDUMP_TYPE>(
          MiniDumpWithFullMemory |
          MiniDumpWithThreadInfo |
          MiniDumpWithProcessThreadData |
          MiniDumpWithFullMemoryInfo |
          MiniDumpWithHandleData |
          MiniDumpWithUnloadedModules
          );
      }
      else
      {
        minidump_type = static_cast<MINIDUMP_TYPE>(
          MiniDumpNormal |
          MiniDumpWithThreadInfo |
          MiniDumpWithProcessThreadData |
          MiniDumpWithDataSegs |
          MiniDumpWithHandleData |
          MiniDumpWithIndirectlyReferencedMemory
          );
      }

      // Fill up the memory dump and close the file handle
      MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle.get(), minidump_type, &dump_exc_info, nullptr, nullptr);
      FlushFileBuffers(file_handle.get());
      file_handle.close();
    }

    s32 report_crash(void* exceptionInfo, s32 numFramesToSkip)
    {
      rsl::unique_lock lock(crash_assert_mtx());

      LPEXCEPTION_POINTERS exception_info = static_cast<LPEXCEPTION_POINTERS>(exceptionInfo);

      // This code is 7 frames after the actual crash, coming from a structured exception handling block
      auto stack = rsl::stacktrace::current(numFramesToSkip);

      // We cannot allocate heap memory as we have possibly run out of memory
      // Therefore we cannot use any logging and need to perform all operations on the stack
      REX_ERROR(CrashHandlingLog, "Error occurred!");
      for (const rsl::stacktrace_entry& line : stack)
      {
        REX_ERROR(CrashHandlingLog, line.description());
      }

      // If a debugger is attached, we will have triggered a breakpoint at the point of the crash
      // However, if a crash dumps needs to get created, we reach this point and the user has the possibility
      // to create a crash dump by dragging the instruction pointer through an IDE.
      // 
      // If no debugger is attached however, we always create a crash dump.
      if (rex::is_debugger_attached())
      {
        DEBUG_BREAK();
      }
      else
      {
        rsl::string crash_dump_path = path::join(vfs::session_data_root(), "crash.dmp");
        create_dump(crash_dump_path, exception_info);
        REX_ERROR(CrashHandlingLog, "Created crash dump at: {}", crash_dump_path);
      }

      return EXCEPTION_EXECUTE_HANDLER;
    }

    s32 report_crash_from_main_thread(void* exceptionInfo)
    {
      const s32 num_frames_to_skip = 8;
      return report_crash(exceptionInfo, num_frames_to_skip);
    }
    s32 report_crash_from_thread(void* exceptionInfo)
    {
      const s32 num_frames_to_skip = 7;
      return report_crash(exceptionInfo, num_frames_to_skip);
    }
  }
}