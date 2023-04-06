

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include <cstdio>

#ifdef _WIN32
  #include "rex_engine/diagnostics/logging/internal/details/windows_include.h"
#endif

namespace rexlog
{

  namespace sinks
  {

    template <typename ConsoleMutex>
    class StdoutSinkBase : public sink
    {
    public:
      using mutex_t = typename ConsoleMutex::mutex_t;
      explicit StdoutSinkBase(FILE* file);
      ~StdoutSinkBase() override = default;

      StdoutSinkBase(const StdoutSinkBase& other) = delete;
      StdoutSinkBase(StdoutSinkBase&& other)      = delete;

      StdoutSinkBase& operator=(const StdoutSinkBase& other) = delete;
      StdoutSinkBase& operator=(StdoutSinkBase&& other)      = delete;

      void log(const details::LogMsg& msg) override;
      void flush() override;
      void set_pattern(const rex::DebugString& pattern) override;

      void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) override;

    protected:
      mutex_t& m_mutex;
      FILE* m_file;
      rsl::unique_ptr<rexlog::formatter> m_formatter;
#ifdef _WIN32
      HANDLE m_handle;
#endif // WIN32
    };

    template <typename ConsoleMutex>
    class StdoutSink : public StdoutSinkBase<ConsoleMutex>
    {
    public:
      StdoutSink();
    };

    template <typename ConsoleMutex>
    class StderrSink : public StdoutSinkBase<ConsoleMutex>
    {
    public:
      StderrSink();
    };

    using stdout_sink_mt = StdoutSink<details::ConsoleMutex>;
    using stdout_sink_st = StdoutSink<details::ConsoleNullMutex>;

    using stderr_sink_mt = StderrSink<details::ConsoleMutex>;
    using stderr_sink_st = StderrSink<details::ConsoleNullMutex>;

  } // namespace sinks

  // factory methods
  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stdout_logger_mt(const rex::DebugString& loggerName);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stdout_logger_st(const rex::DebugString& loggerName);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stderr_logger_mt(const rex::DebugString& loggerName);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stderr_logger_st(const rex::DebugString& loggerName);

} // namespace rexlog