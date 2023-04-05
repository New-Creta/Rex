

#pragma once

#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"
#include "rex_std/iterator.h"

#include <cstdio>
#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

namespace rexlog
{

  // public methods
  REXLOG_INLINE logger::logger(const logger& other)
      : name_(other.name_)
      , sinks_(other.sinks_)
      , level_(other.level_.load(rsl::memory_order_relaxed))
      , flush_level_(other.flush_level_.load(rsl::memory_order_relaxed))
      , custom_err_handler_(other.custom_err_handler_)
      , tracer_(other.tracer_)
  {
  }

  REXLOG_INLINE logger::logger(logger&& other) REXLOG_NOEXCEPT : name_(rsl::move(other.name_)),
                                                                 sinks_(rsl::move(other.sinks_)),
                                                                 level_(other.level_.load(rsl::memory_order_relaxed)),
                                                                 flush_level_(other.flush_level_.load(rsl::memory_order_relaxed)),
                                                                 custom_err_handler_(rsl::move(other.custom_err_handler_)),
                                                                 tracer_(rsl::move(other.tracer_))

  {
  }

  REXLOG_INLINE logger& logger::operator=(logger other) REXLOG_NOEXCEPT
  {
    this->swap(other);
    return *this;
  }

  REXLOG_INLINE void logger::swap(rexlog::logger& other) REXLOG_NOEXCEPT
  {
    name_.swap(other.name_);
    sinks_.swap(other.sinks_);

    // swap level_
    auto other_level = other.level_.load();
    auto my_level    = level_.exchange(other_level);
    other.level_.store(my_level);

    // swap flush level_
    other_level = other.flush_level_.load();
    my_level    = flush_level_.exchange(other_level);
    other.flush_level_.store(my_level);

    custom_err_handler_.swap(other.custom_err_handler_);
    rsl::swap(tracer_, other.tracer_);
  }

  REXLOG_INLINE void swap(logger& a, logger& b)
  {
    a.swap(b);
  }

  REXLOG_INLINE void logger::set_level(level::level_enum log_level)
  {
    level_.store(log_level);
  }

  REXLOG_INLINE level::level_enum logger::level() const
  {
    return static_cast<level::level_enum>(level_.load(rsl::memory_order_relaxed));
  }

  REXLOG_INLINE const rex::DebugString& logger::name() const
  {
    return name_;
  }

  // set formatting for the sinks in this logger.
  // each sink will get a separate instance of the formatter object.
  REXLOG_INLINE void logger::set_formatter(rsl::unique_ptr<formatter> f)
  {
    for(auto it = sinks_.begin(); it != sinks_.end(); ++it)
    {
      if(++it == sinks_.end())
      {
        // last element - we can be move it.
        (*it)->set_formatter(rsl::move(f));
        break; // to prevent clang-tidy warning
      }
      else
      {
        (*it)->set_formatter(f->clone());
      }
    }
  }

  REXLOG_INLINE void logger::set_pattern(rex::DebugString pattern, pattern_time_type time_type)
  {
    auto new_formatter = details::make_unique<pattern_formatter>(rsl::move(pattern), time_type);
    set_formatter(rsl::move(new_formatter));
  }

  // create new backtrace sink and move to it all our child sinks
  REXLOG_INLINE void logger::enable_backtrace(size_t n_messages)
  {
    tracer_.enable(n_messages);
  }

  // restore orig sinks and level and delete the backtrace sink
  REXLOG_INLINE void logger::disable_backtrace()
  {
    tracer_.disable();
  }

  REXLOG_INLINE void logger::dump_backtrace()
  {
    dump_backtrace_impl();
  }

  // flush functions
  REXLOG_INLINE void logger::flush()
  {
    flush_impl();
  }

  REXLOG_INLINE void logger::flush_on(level::level_enum log_level)
  {
    flush_level_.store(log_level);
  }

  REXLOG_INLINE level::level_enum logger::flush_level() const
  {
    return static_cast<level::level_enum>(flush_level_.load(rsl::memory_order_relaxed));
  }

  // sinks
  REXLOG_INLINE const rex::DebugVector<sink_ptr>& logger::sinks() const
  {
    return sinks_;
  }

  REXLOG_INLINE rex::DebugVector<sink_ptr>& logger::sinks()
  {
    return sinks_;
  }

  // error handler
  REXLOG_INLINE void logger::set_error_handler(err_handler handler)
  {
    custom_err_handler_ = rsl::move(handler);
  }

  // create new logger with same sinks and configuration.
  REXLOG_INLINE rsl::shared_ptr<logger> logger::clone(rex::DebugString logger_name)
  {
    auto cloned   = rsl::allocate_shared<logger>(rex::global_debug_allocator(), *this);
    cloned->name_ = rsl::move(logger_name);
    return cloned;
  }

  // protected methods
  REXLOG_INLINE void logger::log_it_impl(const rexlog::details::LogMsg& LogMsg, bool log_enabled, bool traceback_enabled)
  {
    if(log_enabled)
    {
      sink_it_impl(LogMsg);
    }
    if(traceback_enabled)
    {
      tracer_.push_back(LogMsg);
    }
  }

  REXLOG_INLINE void logger::sink_it_impl(const details::LogMsg& msg)
  {
    for(auto& sink: sinks_)
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

  REXLOG_INLINE void logger::flush_impl()
  {
    for(auto& sink: sinks_)
    {
      REXLOG_TRY
      {
        sink->flush();
      }
      REXLOG_LOGGER_CATCH(source_loc())
    }
  }

  REXLOG_INLINE void logger::dump_backtrace_impl()
  {
    using details::LogMsg;
    if(tracer_.enabled() && !tracer_.empty())
    {
      sink_it_impl(LogMsg {name(), level::info, "****************** Backtrace Start ******************"});
      tracer_.foreach_pop([this](const LogMsg& msg) { this->sink_it_impl(msg); });
      sink_it_impl(LogMsg {name(), level::info, "****************** Backtrace End ********************"});
    }
  }

  REXLOG_INLINE bool logger::should_flush_impl(const details::LogMsg& msg)
  {
    auto flush_level = flush_level_.load(rsl::memory_order_relaxed);
    return (msg.level >= flush_level) && (msg.level != level::off);
  }

  REXLOG_INLINE void logger::err_handler_impl(const rex::DebugString& msg)
  {
    if(custom_err_handler_)
    {
      custom_err_handler_(msg);
    }
    else
    {
      using rsl::chrono::system_clock;
      static rsl::mutex mutex;
      static rsl::chrono::system_clock::time_point last_report_time;
      static size_t err_counter = 0;
      rsl::unique_lock<rsl::mutex> lk {mutex};
      auto now = system_clock::now();
      err_counter++;
      if(now - last_report_time < rsl::chrono::seconds(1))
      {
        return;
      }
      last_report_time = now;
      auto tm_time     = details::os::localtime(system_clock::to_time_t(now));
      char date_buf[64];
      strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &tm_time);
#if defined(USING_R) && defined(R_R_H) // if in R environment
      REprintf("[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf, name().c_str(), msg.c_str());
#else
      fprintf(stderr, "[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf, name().c_str(), msg.c_str());
#endif
    }
  }
} // namespace rexlog
