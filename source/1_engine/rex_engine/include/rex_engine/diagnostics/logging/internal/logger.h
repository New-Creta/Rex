

#pragma once

// Thread safe Logger (except for set_error_handler())
// Has name, log level, vector of rsl::shared sink pointers and formatter
// Upon each log write the Logger:
// 1. Checks if its log level is enough to log the message and if yes:
// 2. Call the underlying sinks to do the job.
// 3. Each sink use its own private copy of a formatter to format the message
// and send to its destination.
//
// The use of private formatter per sink provides the opportunity to cache some
// formatted data, and support for different format per sink.

#include "rex_engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_std/atomic.h"
#include "rex_std/format.h"

namespace rexlog
{
    using Sinks = rex::DebugVector<sink_ptr>;

    class Logger
    {
    public:
        explicit Logger(rex::DebugString name);

        Logger(rex::DebugString name, sink_ptr singleSink);
        Logger(rex::DebugString name, sinks_init_list sinks);
        
        template <typename It>
        Logger(rex::DebugString name, It begin, It end)
            : m_name(rsl::move(name))
            , m_sinks(begin, end)
        {
        }

        virtual ~Logger() = default;

        Logger(const Logger& other);
        Logger(Logger&& other) noexcept;
        Logger& operator=(const Logger& other) noexcept;
        Logger& operator=(Logger&& other) noexcept;


    public:
        void                                set_level(level::LevelEnum logLevel);
        void                                set_formatter(PatternFormatter f);
        void                                set_pattern(const rsl::small_stack_string& pattern, PatternTimeType timeType = PatternTimeType::Local);

        bool                                should_log(level::LevelEnum msgLevel) const;

        const rex::DebugString&             name() const;

        level::LevelEnum                    level() const;
        level::LevelEnum                    flush_level() const;

        void                                flush();
        void                                flush_on(level::LevelEnum logLevel);

        const Sinks&                        sinks() const;
        Sinks&                              sinks();

        virtual rsl::shared_ptr<Logger>     clone(rex::DebugString loggerName);

        void                                swap(rexlog::Logger& other) noexcept;

    public:
        void log(log_clock::time_point logTime, rsl::source_location loc, level::LevelEnum lvl, string_view_t msg);
        void log(rsl::source_location loc, level::LevelEnum lvl, string_view_t msg);
        void log(level::LevelEnum lvl, string_view_t msg);

        // T cannot be statically converted to format string (including string_view/wstring_view)
        template <class T, typename rsl::enable_if<!IsConvertibleToAnyFormatString<const T&>::value, int>::type = 0>
        void log(rsl::source_location loc, level::LevelEnum lvl, const T& msg)
        {
            log(loc, lvl, "{}", msg);
        }

        template <typename... Args>
        void log(rsl::source_location loc, level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
        {
            log_impl(loc, lvl, details::to_string_view(fmt), rsl::forward<Args>(args)...);
        }
        template <typename... Args>
        void log(level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
        {
            log(rsl::source_location{}, lvl, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void log(level::LevelEnum lvl, const T& msg)
        {
            log(rsl::source_location{}, lvl, msg);
        }

        template <typename... Args>
        void trace(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Trace, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void trace(const T& msg)
        {
            log(level::LevelEnum::Trace, msg);
        }

        template <typename... Args>
        void debug(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Debug, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void debug(const T& msg)
        {
            log(level::LevelEnum::Debug, msg);
        }

        template <typename... Args>
        void info(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Info, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void info(const T& msg)
        {
            log(level::LevelEnum::Info, msg);
        }

        template <typename... Args>
        void warn(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Warn, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void warn(const T& msg)
        {
            log(level::LevelEnum::Warn, msg);
        }

        template <typename... Args>
        void error(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Err, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void error(const T& msg)
        {
            log(level::LevelEnum::Err, msg);
        }

        template <typename... Args>
        void critical(format_string_t<Args...> fmt, Args&&... args)
        {
            log(level::LevelEnum::Critical, fmt, rsl::forward<Args>(args)...);
        }
        template <typename T>
        void critical(const T& msg)
        {
            log(level::LevelEnum::Critical, msg);
        }

    protected:
        // common implementation for after templated public api has been resolved
        template <typename... Args>
        void log_impl(rsl::source_location loc, level::LevelEnum lvl, string_view_t fmt, Args&&... args)
        {
            const bool log_enabled = should_log(lvl);
            if (!log_enabled)
            {
                return;
            }

            memory_buf_t buf;
            rsl::vformat_to(rsl::back_inserter(buf), fmt, rsl::make_format_args(rsl::forward<Args>(args)...));

            const details::LogMsg log_msg(loc, m_name, lvl, string_view_t(buf.data(), buf.size()));
            log_it_impl(log_msg, log_enabled);
        }

    protected:
        bool                should_flush_impl(const details::LogMsg& msg);

        void                log_it_impl(const details::LogMsg& logMsg, bool logEnabled);
        virtual void        sink_it_impl(const details::LogMsg& msg);
        virtual void        flush_it_impl();

        void                set_name(rex::DebugString&& name);

    private:
        rex::DebugString    m_name;
        Sinks               m_sinks;
        rexlog::level_t     m_level{ (int32)level::LevelEnum::Info };
        rexlog::level_t     m_flush_level{ (int32)level::LevelEnum::Off };
    };

    void swap(Logger& a, Logger& b);

    namespace internal
    {
        // Create and register a Logger with a templated sink type
        // The Logger's level, formatter and flush level will be set according the
        // global settings.
        //
        // Example:
        //   rexlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
        template <typename Sink, typename... SinkArgs>
        static rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... args)
        {
            auto sink = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
            auto new_logger = rsl::allocate_shared<rexlog::Logger>(rex::global_debug_allocator(), rsl::move(loggerName), rsl::move(sink));
            details::Registry::instance().initialize_logger(new_logger);
            return new_logger;
        }
    };


    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return internal::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }

} // namespace rexlog