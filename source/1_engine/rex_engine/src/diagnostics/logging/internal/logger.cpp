#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"
#include "rex_std/iterator.h"

#include <cstdio>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    //-------------------------------------------------------------------------
    Logger::Logger(rsl::string_view name)
        : m_name(rex::DebugString(name))
        , m_sinks()
    {
    }

    //-------------------------------------------------------------------------
    Logger::Logger(rsl::string_view name, sink_ptr singleSink)
        : Logger(name, { rsl::move(singleSink) })
    {
    }

    //-------------------------------------------------------------------------
    Logger::Logger(rsl::string_view name, sinks_init_list sinks)
        : Logger(name, sinks.begin(), sinks.end())
    {
    }

    //-------------------------------------------------------------------------
    Logger::Logger(const Logger& other)
        : m_name(other.m_name)
        , m_sinks(other.m_sinks)
        , m_level(other.m_level.load(rsl::memory_order_relaxed))
        , m_flush_level(other.m_flush_level.load(rsl::memory_order_relaxed))
    {
    }

    //-------------------------------------------------------------------------
    Logger::Logger(Logger&& other) noexcept 
        : m_name(rsl::move(other.m_name))
        , m_sinks(rsl::move(other.m_sinks))
        , m_level(other.m_level.load(rsl::memory_order_relaxed))
        , m_flush_level(other.m_flush_level.load(rsl::memory_order_relaxed))
    {
    }

    //-------------------------------------------------------------------------
    Logger::~Logger()
    {

    }

    //-------------------------------------------------------------------------
    Logger& Logger::operator=(const Logger& other) noexcept
    {
        Logger tmp(other);
        this->swap(tmp);
        return *this;
    }

    //-------------------------------------------------------------------------
    Logger& Logger::operator=(Logger&& other) noexcept
    {
        this->swap(other);
        return *this;
    }

    //-------------------------------------------------------------------------
    bool Logger::should_log(level::LevelEnum msgLevel) const
    {
        return (int32)msgLevel >= m_level.load(rsl::memory_order_relaxed);
    }

    //-------------------------------------------------------------------------
    void Logger::log(log_clock::time_point logTime, rsl::source_location loc, level::LevelEnum lvl, rsl::string_view msg)
    {
        const bool log_enabled = should_log(lvl);
        if (!log_enabled)
        {
            return;
        }

        details::LogMsg const log_msg(logTime, loc, m_name, lvl, msg);
        log_it_impl(log_msg, log_enabled);
    }

    //-------------------------------------------------------------------------
    void Logger::log(rsl::source_location loc, level::LevelEnum lvl, rsl::string_view msg)
    {
        const bool log_enabled = should_log(lvl);
        if (!log_enabled)
        {
            return;
        }

        details::LogMsg const log_msg(loc, m_name, lvl, msg);
        log_it_impl(log_msg, log_enabled);
    }

    //-------------------------------------------------------------------------
    void Logger::log(level::LevelEnum lvl, rsl::string_view msg)
    {
        log(rsl::source_location{}, lvl, msg);
    }

    //-------------------------------------------------------------------------
    void Logger::set_level(level::LevelEnum logLevel)
    {
        m_level.store((int32)logLevel);
    }

    //-------------------------------------------------------------------------
    void Logger::set_formatter(PatternFormatter f)
    {
        for (auto it = m_sinks.begin(); it != m_sinks.end(); ++it)
        {
            auto& sink = *it;
            if (++it == m_sinks.end())
            {
                // last element - we can be move it.
                sink->set_formatter(rsl::move(f));
                break; // to prevent clang-tidy warning
            }
            else
            {
                (*it)->set_formatter(f.clone());
            }
        }
    }

    //-------------------------------------------------------------------------
    void Logger::set_pattern(rsl::string_view pattern, PatternTimeType timeType)
    {
        auto new_formatter = PatternFormatter(pattern, timeType);
        set_formatter(rsl::move(new_formatter));
    }

    //-------------------------------------------------------------------------
    rsl::string_view Logger::name() const
    {
        return m_name;
    }

    //-------------------------------------------------------------------------
    level::LevelEnum Logger::level() const
    {
        return static_cast<level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
    }

    //-------------------------------------------------------------------------
    level::LevelEnum Logger::flush_level() const
    {
        return static_cast<level::LevelEnum>(m_flush_level.load(rsl::memory_order_relaxed));
    }

    //-------------------------------------------------------------------------
    void Logger::flush()
    {
        flush_it_impl();
    }

    //-------------------------------------------------------------------------
    void Logger::flush_on(level::LevelEnum logLevel)
    {
        m_flush_level.store((int32)logLevel);
    }

    //-------------------------------------------------------------------------
    const Sinks& Logger::sinks() const
    {
        return m_sinks;
    }

    //-------------------------------------------------------------------------
    Sinks& Logger::sinks()
    {
        return m_sinks;
    }

    //-------------------------------------------------------------------------
    rsl::shared_ptr<Logger> Logger::clone(rsl::string_view loggerName)
    {
        auto cloned = rsl::allocate_shared<Logger>(rex::global_debug_allocator(), *this);
        cloned->m_name = rex::DebugString(loggerName);
        return cloned;
    }

    //-------------------------------------------------------------------------
    void Logger::swap(rexlog::Logger& other) noexcept
    {
        m_name.swap(other.m_name);
        m_sinks.swap(other.m_sinks);

        // swap level_
        auto other_level = other.m_level.load();
        auto my_level = m_level.exchange(other_level);
        other.m_level.store(my_level);

        // swap flush level_
        other_level = other.m_flush_level.load();
        my_level = m_flush_level.exchange(other_level);
        other.m_flush_level.store(my_level);
    }

    //-------------------------------------------------------------------------
    bool Logger::should_flush_impl(const details::LogMsg& msg)
    {
        auto flush_level = m_flush_level.load(rsl::memory_order_relaxed);
        return ((int32)msg.level() >= flush_level) && (msg.level() != level::LevelEnum::Off);
    }

    //-------------------------------------------------------------------------
    void Logger::log_it_impl(const rexlog::details::LogMsg& logMsg, bool logEnabled)
    {
        if (logEnabled)
        {
            sink_it_impl(logMsg);
        }
    }

    //-------------------------------------------------------------------------
    void Logger::sink_it_impl(const details::LogMsg& msg)
    {
        for (auto& sink : m_sinks)
        {
            if (sink->should_log(msg.level()))
            {
                sink->log(msg);
            }
        }

        if (should_flush_impl(msg))
        {
            flush_it_impl();
        }
    }

    //-------------------------------------------------------------------------
    void Logger::flush_it_impl()
    {
        for (auto& sink : m_sinks)
        {
            sink->flush();
        }
    }

    //-------------------------------------------------------------------------
    void Logger::set_name(rsl::string_view name)
    {
        m_name = rex::DebugString(name);
    }

    //-------------------------------------------------------------------------
    void swap(Logger& a, Logger& b)
    {
        a.swap(b);
    }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
