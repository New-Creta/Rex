

#pragma once

#include "rex_engine/memory/global_allocator.h"
#include "rex_std/memory.h"

#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_sinks.h"

#ifdef _WIN32
  // under windows using fwrite to non-binary stream results in \r\r\n (see issue #1675)
  // so instead we use ::FileWrite
  #include "rex_engine/diagnostics/logging/internal/details/windows_include.h"

  #ifndef _USING_V110_SDK71_ // fileapi.h doesn't exist in winxp
    #include <fileapi.h"     // WriteFile (..)
  #endif

  #include <io.h"    // _get_osfhandle(..)
  #include <stdio.h" // _fileno(..)
#endif               // WIN32

namespace rexlog
{

  namespace sinks
  {

    template <typename ConsoleMutex>
    REXLOG_INLINE stdout_sink_base<ConsoleMutex>::stdout_sink_base(FILE* file)
        : m_mutex(ConsoleMutex::mutex())
        , file_(file)
        , m_formatter(details::make_unique<rexlog::pattern_formatter>())
    {
#ifdef _WIN32
      // get windows handle from the FILE* object

      handle_ = reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(file_)));

      // don't throw to support cases where no console is attached,
      // and let the log method to do nothing if (handle_ == INVALID_HANDLE_VALUE).
      // throw only if non stdout/stderr target is requested (probably regular file and not console).
      if(handle_ == INVALID_HANDLE_VALUE && file != stdout && file != stderr)
      {
        throw_rexlog_ex(rex::DebugString("rexlog::stdout_sink_base: _get_osfhandle() failed"), errno);
      }
#endif // WIN32
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void stdout_sink_base<ConsoleMutex>::log(const details::LogMsg& msg)
    {
#ifdef _WIN32
      if(handle_ == INVALID_HANDLE_VALUE)
      {
        return;
      }
      rsl::unique_lock<mutex_t> lock(m_mutex);
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      ::fflush(file_); // flush in case there is something in this file_ already
      auto size           = static_cast<DWORD>(formatted.size());
      DWORD bytes_written = 0;
      bool ok             = ::WriteFile(handle_, formatted.data(), size, &bytes_written, nullptr) != 0;
      if(!ok)
      {
        rex::DebugString err(rex::global_debug_allocator());
        err += "stdout_sink_base: WriteFile() failed. GetLastError(): ";
        err += rsl::to_string(::GetLastError());
        throw_rexlog_ex(err);
      }
#else
      rsl::unique_lock<mutex_t> lock(m_mutex);
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      ::fwrite(formatted.data(), sizeof(char), formatted.size(), file_);
      ::fflush(file_); // flush every line to terminal
#endif // WIN32
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void stdout_sink_base<ConsoleMutex>::flush()
    {
      rsl::unique_lock<mutex_t> lock(m_mutex);
      fflush(file_);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void stdout_sink_base<ConsoleMutex>::set_pattern(const rex::DebugString& pattern)
    {
      rsl::unique_lock<mutex_t> lock(m_mutex);
      m_formatter = rsl::make_unique<rexlog::pattern_formatter>(pattern);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void stdout_sink_base<ConsoleMutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter)
    {
      rsl::unique_lock<mutex_t> lock(m_mutex);
      m_formatter = rsl::move(sink_formatter);
    }

    // stdout sink
    template <typename ConsoleMutex>
    REXLOG_INLINE stdout_sink<ConsoleMutex>::stdout_sink()
        : stdout_sink_base<ConsoleMutex>(stdout)
    {
    }

    // stderr sink
    template <typename ConsoleMutex>
    REXLOG_INLINE stderr_sink<ConsoleMutex>::stderr_sink()
        : stdout_sink_base<ConsoleMutex>(stderr)
    {
    }

  } // namespace sinks

  // factory methods
  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stdout_logger_mt(const rex::DebugString& logger_name)
  {
    return Factory::template create<sinks::stdout_sink_mt>(rex::DebugString(logger_name));
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stdout_logger_st(const rex::DebugString& logger_name)
  {
    return Factory::template create<sinks::stdout_sink_st>(rex::DebugString(logger_name));
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stderr_logger_mt(const rex::DebugString& logger_name)
  {
    return Factory::template create<sinks::stderr_sink_mt>(rex::DebugString(logger_name));
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stderr_logger_st(const rex::DebugString& logger_name)
  {
    return Factory::template create<sinks::stderr_sink_st>(rex::DebugString(logger_name));
  }
} // namespace rexlog
