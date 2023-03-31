

#pragma once

#include <rex_engine/diagnostics/logging/internal/rexlog.h>

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter.h>

namespace rexlog {

REXLOG_INLINE void initialize_logger(rsl::shared_ptr<logger> logger)
{
    details::registry::instance().initialize_logger(rsl::move(logger));
}

REXLOG_INLINE rsl::shared_ptr<logger> get(const rsl::string &name)
{
    return details::registry::instance().get(name);
}

REXLOG_INLINE void set_formatter(rsl::unique_ptr<rexlog::formatter> formatter)
{
    details::registry::instance().set_formatter(rsl::move(formatter));
}

REXLOG_INLINE void set_pattern(rsl::string pattern, pattern_time_type time_type)
{
    set_formatter(rsl::unique_ptr<rexlog::formatter>(new pattern_formatter(rsl::move(pattern), time_type)));
}

REXLOG_INLINE void enable_backtrace(size_t n_messages)
{
    details::registry::instance().enable_backtrace(n_messages);
}

REXLOG_INLINE void disable_backtrace()
{
    details::registry::instance().disable_backtrace();
}

REXLOG_INLINE void dump_backtrace()
{
    default_logger_raw()->dump_backtrace();
}

REXLOG_INLINE level::level_enum get_level()
{
    return default_logger_raw()->level();
}

REXLOG_INLINE bool should_log(level::level_enum log_level)
{
    return default_logger_raw()->should_log(log_level);
}

REXLOG_INLINE void set_level(level::level_enum log_level)
{
    details::registry::instance().set_level(log_level);
}

REXLOG_INLINE void flush_on(level::level_enum log_level)
{
    details::registry::instance().flush_on(log_level);
}

REXLOG_INLINE void set_error_handler(void (*handler)(const rsl::string &msg))
{
    details::registry::instance().set_error_handler(handler);
}

REXLOG_INLINE void register_logger(rsl::shared_ptr<logger> logger)
{
    details::registry::instance().register_logger(rsl::move(logger));
}

REXLOG_INLINE void apply_all(const rsl::function<void(rsl::shared_ptr<logger>)> &fun)
{
    details::registry::instance().apply_all(fun);
}

REXLOG_INLINE void drop(const rsl::string &name)
{
    details::registry::instance().drop(name);
}

REXLOG_INLINE void drop_all()
{
    details::registry::instance().drop_all();
}

REXLOG_INLINE void shutdown()
{
    details::registry::instance().shutdown();
}

REXLOG_INLINE void set_automatic_registration(bool automatic_registration)
{
    details::registry::instance().set_automatic_registration(automatic_registration);
}

REXLOG_INLINE rsl::shared_ptr<rexlog::logger> default_logger()
{
    return details::registry::instance().default_logger();
}

REXLOG_INLINE rexlog::logger *default_logger_raw()
{
    return details::registry::instance().get_default_raw();
}

REXLOG_INLINE void set_default_logger(rsl::shared_ptr<rexlog::logger> default_logger)
{
    details::registry::instance().set_default_logger(rsl::move(default_logger));
}

REXLOG_INLINE void apply_logger_env_levels(rsl::shared_ptr<logger> logger)
{
    details::registry::instance().apply_logger_env_levels(rsl::move(logger));
}

} // namespace rexlog
