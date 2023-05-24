

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/details/windows_include.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/memory.h"

// under windows using fwrite to non-binary stream results in \r\r\n (see issue #1675)
// so instead we use ::FileWrite
#include "rex_engine/diagnostics/logging/internal/details/windows_include.h"

#include <fileapi.h>        // WriteFile (..)
#include <io.h>             // _get_osfhandle(..)
#include <stdio.h>          // _fileno(..)
#include <cstdio>

namespace rexlog
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
            StdoutSinkBase(StdoutSinkBase&& other) = delete;

            StdoutSinkBase& operator=(const StdoutSinkBase& other) = delete;
            StdoutSinkBase& operator=(StdoutSinkBase&& other) = delete;

            void log(const details::LogMsg& msg) override;
            void flush() override;
            void set_pattern(const rex::DebugString& pattern) override;

            void set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter) override;

        private:
            mutex_t* m_mutex;
            FILE* m_file;
            rsl::unique_ptr<rexlog::formatter> m_formatter;
            HANDLE m_handle;
        };

        template <typename ConsoleMutex>
        StdoutSinkBase<ConsoleMutex>::StdoutSinkBase(FILE* file)
            : m_mutex(&ConsoleMutex::mutex())
            , m_file(file)
            , m_formatter(details::make_unique<rexlog::PatternFormatter>())
            , m_handle(reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(m_file)))) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
        {
            // get windows handle from the FILE* object

            // don't throw to support cases where no console is attached,
            // and let the log method to do nothing if (handle_ == INVALID_HANDLE_VALUE).
            // throw only if non stdout/stderr target is requested (probably regular file and not console).
            if (m_handle == INVALID_HANDLE_VALUE && file != stdout && file != stderr)
            {
                throw_rexlog_ex(rex::DebugString("rexlog::stdout_sink_base: _get_osfhandle() failed"), errno);
            }
        }

        template <typename ConsoleMutex>
        void StdoutSinkBase<ConsoleMutex>::log(const details::LogMsg& msg)
        {
            if (m_handle == INVALID_HANDLE_VALUE)
            {
                return;
            }

            const rsl::unique_lock<mutex_t> lock(*m_mutex);
            memory_buf_t formatted;
            m_formatter->format(msg, formatted);
            REX_ASSERT_X(::fflush(m_file), "Failed to flush buffer"); // flush in case there is something in this file_ already
            auto size = static_cast<DWORD>(formatted.size());
            DWORD bytes_written = 0;
            const bool ok = ::WriteFile(m_handle, formatted.data(), size, &bytes_written, nullptr) != 0;
            if (!ok)
            {
                rex::DebugString err(rex::global_debug_allocator());
                err += "stdout_sink_base: WriteFile() failed. GetLastError(): ";
                err += rsl::to_string(::GetLastError());
                throw_rexlog_ex(err);
            }
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