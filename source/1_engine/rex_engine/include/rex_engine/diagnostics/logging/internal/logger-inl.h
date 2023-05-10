

#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"
#include "rex_std/iterator.h"

#include <cstdio>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{

  // public methods
  REXLOG_INLINE Logger::Logger(const Logger& other)
      : m_name(other.m_name)
      , m_sinks(other.m_sinks)
      , m_level(other.m_level.load(rsl::memory_order_relaxed))
      , m_flush_level(other.m_flush_level.load(rsl::memory_order_relaxed))
      , m_custom_err_handler(other.m_custom_err_handler)
      , m_tracer(other.m_tracer)
  {
  }

  REXLOG_INLINE Logger::Logger(Logger&& other) REXLOG_NOEXCEPT : m_name(rsl::move(other.m_name)),
                                                                 m_sinks(rsl::move(other.m_sinks)),
                                                                 m_level(other.m_level.load(rsl::memory_order_relaxed)),
                                                                 m_flush_level(other.m_flush_level.load(rsl::memory_order_relaxed)),
                                                                 m_custom_err_handler(rsl::move(other.m_custom_err_handler)),
                                                                 m_tracer(rsl::move(other.m_tracer))
  {
  }

  REXLOG_INLINE Logger& Logger::operator=(const Logger& other) REXLOG_NOEXCEPT
  {
    Logger tmp(other);
    this->swap(tmp);
    return *this;
  }

  REXLOG_INLINE Logger& Logger::operator=(Logger&& other) REXLOG_NOEXCEPT
  {
    this->swap(other);
    return *this;
  }

  REXLOG_INLINE void Logger::swap(rexlog::Logger& other) REXLOG_NOEXCEPT
  {
    m_name.swap(other.m_name);
    m_sinks.swap(other.m_sinks);

    // swap level_
    auto other_level = other.m_level.load();
    auto my_level    = m_level.exchange(other_level);
    other.m_level.store(my_level);

    // swap flush level_
    other_level = other.m_flush_level.load();
    my_level    = m_flush_level.exchange(other_level);
    other.m_flush_level.store(my_level);

    m_custom_err_handler.swap(other.m_custom_err_handler);
    rsl::swap(m_tracer, other.m_tracer);
  }

  REXLOG_INLINE void swap(Logger& a, Logger& b)
  {
    a.swap(b);
  }

  REXLOG_INLINE void Logger::set_level(level::LevelEnum logLevel)
  {
    m_level.store(logLevel);
  }

  REXLOG_INLINE level::LevelEnum Logger::level() const
  {
    return static_cast<level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
  }

  REXLOG_INLINE const rex::DebugString& Logger::name() const
  {
    return m_name;
  }

  // set formatting for the sinks in this Logger.
  // each sink will get a separate instance of the formatter object.
  REXLOG_INLINE void Logger::set_formatter(rsl::unique_ptr<formatter> f)
  {
    for(auto it = m_sinks.begin(); it != m_sinks.end(); ++it)
    {
      auto& sink = *it;
      if(++it == m_sinks.end())
      {
        // last element - we can be move it.
        sink->set_formatter(rsl::move(f));
        break; // to prevent clang-tidy warning
      }
      else
      {
        (*it)->set_formatter(f->clone());
      }
    }
  }

  REXLOG_INLINE void Logger::set_pattern(rsl::string_view pattern, PatternTimeType timeType)
  {
    auto new_formatter = details::make_unique<PatternFormatter>(pattern, timeType);
    set_formatter(rsl::move(new_formatter));
  }

  // create new backtrace sink and move to it all our child sinks
  REXLOG_INLINE void Logger::enable_backtrace(size_t nMessages)
  {
    m_tracer.enable(nMessages);
  }

  // restore orig sinks and level and delete the backtrace sink
  REXLOG_INLINE void Logger::disable_backtrace()
  {
    m_tracer.disable();
  }

  REXLOG_INLINE void Logger::dump_backtrace()
  {
    dump_backtrace_impl();
  }

  // flush functions
  REXLOG_INLINE void Logger::flush()
  {
    flush_impl();
  }

  REXLOG_INLINE void Logger::flush_on(level::LevelEnum logLevel)
  {
    m_flush_level.store(logLevel);
  }

  REXLOG_INLINE level::LevelEnum Logger::flush_level() const
  {
    return static_cast<level::LevelEnum>(m_flush_level.load(rsl::memory_order_relaxed));
  }

  // sinks
  REXLOG_INLINE const rex::DebugVector<sink_ptr>& Logger::sinks() const
  {
    return m_sinks;
  }

  REXLOG_INLINE rex::DebugVector<sink_ptr>& Logger::sinks()
  {
    return m_sinks;
  }

  // error handler
  REXLOG_INLINE void Logger::set_error_handler(err_handler handler)
  {
    m_custom_err_handler = rsl::move(handler);
  }

  // create new Logger with same sinks and configuration.
  REXLOG_INLINE rsl::shared_ptr<Logger> Logger::clone(rex::DebugString loggerName)
  {
    auto cloned    = rsl::allocate_shared<Logger>(rex::global_debug_allocator(), *this);
    cloned->m_name = rsl::move(loggerName);
    return cloned;
  }

  // protected methods
  REXLOG_INLINE void Logger::log_it_impl(const rexlog::details::LogMsg& logMsg, bool logEnabled, bool tracebackEnabled)
  {
    if(logEnabled)
    {
      sink_it_impl(logMsg);
    }
    if(tracebackEnabled)
    {
      m_tracer.push_back(logMsg);
    }
  }

  REXLOG_INLINE void Logger::sink_it_impl(const details::LogMsg& msg)
  {
    for(auto& sink: m_sinks)
    {
      if(sink->should_log(msg.level))
      {
        REXLOG_TRY
        {
          sink->log(msg);
        }
        REXLOG_LOGGER_CATCH(msg.source)
      }
    }

    if(should_flush_impl(msg))
    {
      flush_impl();
    }
  }

  REXLOG_INLINE void Logger::flush_impl()
  {
    for(auto& sink: m_sinks)
    {
      REXLOG_TRY
      {
        sink->flush();
      }
      REXLOG_LOGGER_CATCH(source_loc())
    }
  }

  REXLOG_INLINE void Logger::dump_backtrace_impl()
  {
    using details::LogMsg;
    if(m_tracer.enabled() && !m_tracer.empty())
    {
      sink_it_impl(LogMsg {name(), level::Info, "****************** Backtrace Start ******************"});
      m_tracer.foreach_pop([this](const LogMsg& msg) { this->sink_it_impl(msg); });
      sink_it_impl(LogMsg {name(), level::Info, "****************** Backtrace End ********************"});
    }
  }

  REXLOG_INLINE bool Logger::should_flush_impl(const details::LogMsg& msg)
  {
    auto flush_level = m_flush_level.load(rsl::memory_order_relaxed);
    return (msg.level >= flush_level) && (msg.level != level::Off);
  }

  REXLOG_INLINE void Logger::err_handler_impl(const rex::DebugString& msg)
  {
    if(m_custom_err_handler)
    {
      m_custom_err_handler(msg);
    }
    else
    {
      using rsl::chrono::system_clock;
      static rsl::mutex mutex;
      static rsl::chrono::system_clock::time_point last_report_time;
      static size_t err_counter = 0;
      const rsl::unique_lock<rsl::mutex> lk {mutex};
      auto now = system_clock::now();
      err_counter++;
      if(now - last_report_time < rsl::chrono::seconds(1))
      {
        return;
      }
      last_report_time = now;
      auto tm_time     = details::os::localtime(system_clock::to_time_t(now));
      rsl::array<char, 64> date_buf; // NOLINT(misc-const-correctness)
      REX_ASSERT_X(strftime(date_buf.data(), date_buf.size(), "%Y-%m-%d %H:%M:%S", &tm_time) != 0, "failed to formate date");
#if defined(USING_R) && defined(R_R_H) // if in R environment
      REprintf("[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf, name().c_str(), msg.c_str());
#else
      REX_ASSERT_X(fprintf(stderr, "[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf.data(), name().c_str(), msg.c_str()) != 0, "failed to format string");
#endif
    }
  }

  REXLOG_INLINE void Logger::set_name(rex::DebugString&& name)
  {
    m_name = rsl::move(name);
  }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
