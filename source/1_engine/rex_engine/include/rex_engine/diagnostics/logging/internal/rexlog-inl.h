

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/rexlog.h"

namespace rexlog
{

  REXLOG_INLINE inline void initialize_logger(rsl::shared_ptr<Logger> Logger)
  {
    details::Registry::instance().initialize_logger(rsl::move(Logger));
  }

  REXLOG_INLINE inline rsl::shared_ptr<Logger> get(const rex::DebugString& name)
  {
    return details::Registry::instance().get(name);
  }

  REXLOG_INLINE inline void set_formatter(rsl::unique_ptr<rexlog::formatter> formatter)
  {
    details::Registry::instance().set_formatter(rsl::move(formatter));
  }

  REXLOG_INLINE inline void set_pattern(const rex::DebugString& pattern, PatternTimeType timeType)
  {
    set_formatter(rsl::unique_ptr<rexlog::formatter>(new PatternFormatter(rsl::move(pattern), timeType)));
  }

  REXLOG_INLINE inline void enable_backtrace(size_t nMessages)
  {
    details::Registry::instance().enable_backtrace(nMessages);
  }

  REXLOG_INLINE inline void disable_backtrace()
  {
    details::Registry::instance().disable_backtrace();
  }

  REXLOG_INLINE inline void dump_backtrace()
  {
    default_logger_raw()->dump_backtrace();
  }

  REXLOG_INLINE inline level::LevelEnum get_level()
  {
    return default_logger_raw()->level();
  }

  REXLOG_INLINE inline bool should_log(level::LevelEnum logLevel)
  {
    return default_logger_raw()->should_log(logLevel);
  }

  REXLOG_INLINE inline void set_level(level::LevelEnum logLevel)
  {
    details::Registry::instance().set_level(logLevel);
  }

  REXLOG_INLINE inline void flush_on(level::LevelEnum logLevel)
  {
    details::Registry::instance().flush_on(logLevel);
  }

  REXLOG_INLINE inline void set_error_handler(void (*handler)(const rex::DebugString& msg))
  {
    details::Registry::instance().set_error_handler(handler);
  }

  REXLOG_INLINE inline void register_logger(rsl::shared_ptr<Logger> Logger)
  {
    details::Registry::instance().register_logger(rsl::move(Logger));
  }

  REXLOG_INLINE inline void apply_all(const rsl::function<void(rsl::shared_ptr<Logger>)>& fun)
  {
    details::Registry::instance().apply_all(fun);
  }

  REXLOG_INLINE inline void drop(const rex::DebugString& name)
  {
    details::Registry::instance().drop(name);
  }

  REXLOG_INLINE inline void drop_all()
  {
    details::Registry::instance().drop_all();
  }

  REXLOG_INLINE inline void shutdown()
  {
    details::Registry::instance().shutdown();
  }

  REXLOG_INLINE inline void set_automatic_registration(bool automaticRegistration)
  {
    details::Registry::instance().set_automatic_registration(automaticRegistration);
  }

  REXLOG_INLINE inline rsl::shared_ptr<rexlog::Logger> default_logger()
  {
    return details::Registry::instance().default_logger();
  }

  REXLOG_INLINE inline rexlog::Logger* default_logger_raw()
  {
    return details::Registry::instance().get_default_raw();
  }

  REXLOG_INLINE inline void set_default_logger(rsl::shared_ptr<rexlog::Logger> defaultLogger)
  {
    details::Registry::instance().set_default_logger(rsl::move(defaultLogger));
  }

  REXLOG_INLINE inline void apply_logger_env_levels(rsl::shared_ptr<Logger> Logger)
  {
    details::Registry::instance().apply_logger_env_levels(rsl::move(Logger));
  }

} // namespace rexlog
