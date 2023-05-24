#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/rexlog.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

namespace rexlog
{
  void initialize_logger(rsl::shared_ptr<Logger> logger)
  {
    details::Registry::instance().initialize_logger(rsl::move(logger));
  }

  rsl::shared_ptr<Logger> get(const rex::DebugString& name)
  {
    return details::Registry::instance().get(name);
  }

  void set_formatter(rsl::unique_ptr<rexlog::formatter> formatter)
  {
    details::Registry::instance().set_formatter(rsl::move(formatter));
  }
#include "rex_std/mutex.h"

  void set_pattern(const rex::DebugString& pattern, PatternTimeType timeType)
  {
    set_formatter(rsl::unique_ptr<rexlog::formatter>(new PatternFormatter(rsl::move(pattern), timeType)));
  }

  void enable_backtrace(size_t nMessages)
  {
    details::Registry::instance().enable_backtrace(nMessages);
  }

  void disable_backtrace()
  {
    details::Registry::instance().disable_backtrace();
  }

  void dump_backtrace()
  {
    default_logger_raw()->dump_backtrace();
  }

  level::LevelEnum get_level()
  {
    return default_logger_raw()->level();
  }

  bool should_log(level::LevelEnum logLevel)
  {
    return default_logger_raw()->should_log(logLevel);
  }

  void set_level(level::LevelEnum logLevel)
  {
    details::Registry::instance().set_level(logLevel);
  }

  void flush_on(level::LevelEnum logLevel)
  {
    details::Registry::instance().flush_on(logLevel);
  }

  void set_error_handler(void (*handler)(const rex::DebugString& msg))
  {
    details::Registry::instance().set_error_handler(handler);
  }

  void register_logger(rsl::shared_ptr<Logger> logger)
  {
    details::Registry::instance().register_logger(rsl::move(logger));
  }

  void apply_all(const rsl::function<void(rsl::shared_ptr<Logger>)>& fun)
  {
    details::Registry::instance().apply_all(fun);
  }

  void drop(const rex::DebugString& name)
  {
    details::Registry::instance().drop(name);
  }

  void drop_all()
  {
    details::Registry::instance().drop_all();
  }

  void shutdown()
  {
    details::Registry::instance().shutdown();
  }

  void set_automatic_registration(bool automaticRegistration)
  {
    details::Registry::instance().set_automatic_registration(automaticRegistration);
  }

  rsl::shared_ptr<rexlog::Logger> default_logger()
  {
    return details::Registry::instance().default_logger();
  }

  rexlog::Logger* default_logger_raw()
  {
    return details::Registry::instance().get_default_raw();
  }

  void set_default_logger(rsl::shared_ptr<rexlog::Logger> defaultLogger)
  {
    details::Registry::instance().set_default_logger(rsl::move(defaultLogger));
  }

  void apply_logger_env_levels(rsl::shared_ptr<Logger> logger)
  {
    details::Registry::instance().apply_logger_env_levels(rsl::move(logger));
  }

} // namespace rexlog

// // template instantiate Logger constructor with sinks init list
// template  rexlog::Logger::Logger(rex::DebugString name, sinks_init_list::iterator begin, sinks_init_list::iterator end);
// template  class rexlog::sinks::BaseSink<rsl::mutex>;
// template  class rexlog::sinks::BaseSink<rexlog::details::NullMutex>;
