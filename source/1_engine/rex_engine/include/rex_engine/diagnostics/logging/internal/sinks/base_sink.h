

#pragma once
//
// base sink templated over a mutex (either dummy or real)
// concrete implementation should override the sink_it_impl() and flush_impl()  methods.
// locking is taken care of in this class - no locking needed by the
// implementers..
//

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"

namespace rexlog
{
    namespace sinks
    {
        template <typename Mutex>
        class BaseSink : public AbstractSink
        {
        public:
            BaseSink();
            explicit BaseSink(rsl::unique_ptr<rexlog::formatter> formatter);
            ~BaseSink() override = default;

            BaseSink(const BaseSink&) = delete;
            BaseSink(BaseSink&&) = delete;

            BaseSink& operator=(const BaseSink&) = delete;
            BaseSink& operator=(BaseSink&&) = delete;

            void log(const details::LogMsg& msg) final;
            void flush() final;
            void set_pattern(const rex::DebugString& pattern) final;
            void set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter) final;

        protected:
            virtual void sink_it_impl(const details::LogMsg& msg) = 0;
            virtual void flush_impl() = 0;
            virtual void set_pattern_impl(const rex::DebugString& pattern);
            virtual void set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sinkFormatter);

            rexlog::formatter* formatter();
            Mutex& mutex();

        private:
            // sink formatter
            rsl::unique_ptr<rexlog::formatter> m_formatter;
            Mutex m_mutex;
        };

        template <typename Mutex>
        rexlog::sinks::BaseSink<Mutex>::BaseSink()
            : m_formatter{ details::make_unique<rexlog::PatternFormatter>() }
        {
        }

        template <typename Mutex>
        rexlog::sinks::BaseSink<Mutex>::BaseSink(rsl::unique_ptr<rexlog::formatter> formatter)
            : m_formatter{ rsl::move(formatter) }
        {
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::log(const details::LogMsg& msg)
        {
            const rsl::unique_lock<Mutex> lock(m_mutex);
            sink_it_impl(msg);
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::flush()
        {
            const rsl::unique_lock<Mutex> lock(m_mutex);
            flush_impl();
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::set_pattern(const rex::DebugString& pattern)
        {
            const rsl::unique_lock<Mutex> lock(m_mutex);
            set_pattern_impl(pattern);
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
        {
            const rsl::unique_lock<Mutex> lock(m_mutex);
            set_formatter_impl(rsl::move(sinkFormatter));
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::set_pattern_impl(const rex::DebugString& pattern)
        {
            set_formatter_impl(details::make_unique<rexlog::PatternFormatter>(pattern));
        }

        template <typename Mutex>
        void  rexlog::sinks::BaseSink<Mutex>::set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
        {
            m_formatter = rsl::move(sinkFormatter);
        }

        template <typename Mutex>
        rexlog::formatter* rexlog::sinks::BaseSink<Mutex>::formatter()
        {
            return m_formatter.get();
        }

        template <typename Mutex>
        Mutex& rexlog::sinks::BaseSink<Mutex>::mutex()
        {
            return m_mutex;
        }
    } // namespace sinks
} // namespace rexlog