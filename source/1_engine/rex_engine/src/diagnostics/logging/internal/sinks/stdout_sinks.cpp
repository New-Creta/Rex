#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_sinks.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/memory.h"

#ifdef _WIN32
  // under windows using fwrite to non-binary stream results in \r\r\n (see issue #1675)
  // so instead we use ::FileWrite
  #include "rex_engine/diagnostics/logging/internal/details/windows_include.h"

  #ifndef _USING_V110_SDK71_ // fileapi.h doesn't exist in winxp
    #include <fileapi.h>     // WriteFile (..)
  #endif

  #include <io.h>    // _get_osfhandle(..)
  #include <stdio.h> // _fileno(..)
#endif               // WIN32

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{

  namespace sinks
  {

    template <typename ConsoleMutex>
     StdoutSinkBase<ConsoleMutex>::StdoutSinkBase(FILE* file)
        : m_mutex(&ConsoleMutex::mutex())
        , m_file(file)
        , m_formatter(details::make_unique<rexlog::PatternFormatter>())
        , m_handle(reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(m_file)))) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
    {
#ifdef _WIN32
      // get windows handle from the FILE* object

      // don't throw to support cases where no console is attached,
      // and let the log method to do nothing if (handle_ == INVALID_HANDLE_VALUE).
      // throw only if non stdout/stderr target is requested (probably regular file and not console).
      if(m_handle == INVALID_HANDLE_VALUE && file != stdout && file != stderr)
      {
        throw_rexlog_ex(rex::DebugString("rexlog::stdout_sink_base: _get_osfhandle() failed"), errno);
      }
#endif // WIN32
    }

    template <typename ConsoleMutex>
     void StdoutSinkBase<ConsoleMutex>::log(const details::LogMsg& msg)
    {
#ifdef _WIN32
      if(m_handle == INVALID_HANDLE_VALUE)
      {
        return;
      }
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      REX_ASSERT_X(::fflush(m_file), "Failed to flush buffer"); // flush in case there is something in this file_ already
      auto size           = static_cast<DWORD>(formatted.size());
      DWORD bytes_written = 0;
      const bool ok       = ::WriteFile(m_handle, formatted.data(), size, &bytes_written, nullptr) != 0;
      if(!ok)
      {
        rex::DebugString err(rex::global_debug_allocator());
        err += "stdout_sink_base: WriteFile() failed. GetLastError(): ";
        err += rsl::to_string(::GetLastError());
        throw_rexlog_ex(err);
      }
#else
      rsl::unique_lock<mutex_t> lock(*m_mutex);
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      ::fwrite(formatted.data(), sizeof(char), formatted.size(), file_);
      ::fflush(file_); // flush every line to terminal
#endif // WIN32
    }

    template <typename ConsoleMutex>
     void StdoutSinkBase<ConsoleMutex>::flush()
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      REX_ASSERT_X(::fflush(m_file), "Failed to flush buffer");
    }

    template <typename ConsoleMutex>
     void StdoutSinkBase<ConsoleMutex>::set_pattern(const rex::DebugString& pattern)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      m_formatter = rsl::make_unique<rexlog::PatternFormatter>(pattern);
    }

    template <typename ConsoleMutex>
     void StdoutSinkBase<ConsoleMutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      m_formatter = rsl::move(sinkFormatter);
    }

    // stdout sink
    template <typename ConsoleMutex>
     StdoutSink<ConsoleMutex>::StdoutSink()
        : StdoutSinkBase<ConsoleMutex>(stdout)
    {
    }

    // stderr sink
    template <typename ConsoleMutex>
     StderrSink<ConsoleMutex>::StderrSink()
        : StdoutSinkBase<ConsoleMutex>(stderr)
    {
    }

  } // namespace sinks

  // factory methods
  template <typename Factory>
   rsl::shared_ptr<Logger> stdout_logger_mt(const rex::DebugString& loggerName)
  {
    return Factory::template create<sinks::stdout_sink_mt>(rex::DebugString(loggerName));
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stdout_logger_st(const rex::DebugString& loggerName)
  {
    return Factory::template create<sinks::stdout_sink_st>(rex::DebugString(loggerName));
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stderr_logger_mt(const rex::DebugString& loggerName)
  {
    return Factory::template create<sinks::stderr_sink_mt>(rex::DebugString(loggerName));
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stderr_logger_st(const rex::DebugString& loggerName)
  {
    return Factory::template create<sinks::stderr_sink_st>(rex::DebugString(loggerName));
  }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
