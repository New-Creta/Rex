#pragma once

#include "rex_engine/diagnostics/logging/internal/details/console_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

#include <cstdio>

namespace rex
{
  namespace log
  {
    namespace sinks
    {

      template <typename ConsoleMutex>
      class StdoutSinkBase : public AbstractSink
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
        void set_pattern(rsl::string_view pattern) override;

        void set_formatter(PatternFormatter sinkFormatter) override;

      private:
        mutex_t* m_mutex;
        FILE* m_file;
        PatternFormatter m_formatter;
      };

      template <typename ConsoleMutex>
      StdoutSinkBase<ConsoleMutex>::StdoutSinkBase(FILE* file)
          : m_mutex(&ConsoleMutex::mutex())
          , m_file(file)
          , m_formatter(rsl::make_unique<rex::log::PatternFormatter>())
          , m_handle(reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(m_file)))) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
      {
      }

      template <typename ConsoleMutex>
      void StdoutSinkBase<ConsoleMutex>::log(const details::LogMsg& msg)
      {
        rsl::unique_lock<mutex_t> lock(*m_mutex);
        rsl::big_stack_string formatted;
        m_formatter->format(msg, formatted);
        ::fwrite(formatted.data(), sizeof(char), formatted.size(), file_);
        ::fflush(file_); // flush every line to terminal
      }

      template <typename ConsoleMutex>
      void StdoutSinkBase<ConsoleMutex>::flush()
      {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        REX_ASSERT_X(::fflush(m_file), "Failed to flush buffer");
      }

      template <typename ConsoleMutex>
      void StdoutSinkBase<ConsoleMutex>::set_pattern(rsl::string_view pattern)
      {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        m_formatter = rsl::make_unique<rex::log::PatternFormatter>(pattern);
      }

      template <typename ConsoleMutex>
      void StdoutSinkBase<ConsoleMutex>::set_formatter(PatternFormatter sinkFormatter)
      {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        m_formatter = rsl::move(sinkFormatter);
      }

      template <typename ConsoleMutex>
      class StdoutSink : public StdoutSinkBase<ConsoleMutex>
      {
      public:
        StdoutSink();
      };

      // stdout sink
      template <typename ConsoleMutex>
      StdoutSink<ConsoleMutex>::StdoutSink()
          : StdoutSinkBase<ConsoleMutex>(stdout)
      {
      }

      template <typename ConsoleMutex>
      class StderrSink : public StdoutSinkBase<ConsoleMutex>
      {
      public:
        StderrSink();
      };

      // stderr sink
      template <typename ConsoleMutex>
      StderrSink<ConsoleMutex>::StderrSink()
          : StdoutSinkBase<ConsoleMutex>(stderr)
      {
      }

      using StdoutSinkMt = StdoutSink<details::ConsoleMutex>;
      using StdoutSinkSt = StdoutSink<details::ConsoleNullMutex>;

      using StderrSinkMt = StderrSink<details::ConsoleMutex>;
      using StderrSinkSt = StderrSink<details::ConsoleNullMutex>;

    } // namespace sinks
  }   // namespace log
} // namespace rex