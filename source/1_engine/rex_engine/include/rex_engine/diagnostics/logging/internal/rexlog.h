

// rexlog main header file.
// see example.cpp for usage example

#ifndef REXLOG_H
#define REXLOG_H

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_std/memory.h"

#include <chrono>
#include <functional>
#include <string>

namespace rexlog
{

  using default_factory = SynchronousFactory;

  // Create and register a Logger with a templated sink type
  // The Logger's level, formatter and flush level will be set according the
  // global settings.
  //
  // Example:
  //   rexlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
  template <typename Sink, typename... SinkArgs>
  inline rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
  {
    return default_factory::create<Sink>(rsl::move(logger_name), rsl::forward<SinkArgs>(sink_args)...);
  }

  // Initialize and register a Logger,
  // formatter and flush level will be set according the global settings.
  //
  // Useful for initializing manually created loggers with the global settings.
  //
  // Example:
  //   auto mylogger = rsl::make_shared<rexlog::Logger>("mylogger", ...);
  //   rexlog::initialize_logger(mylogger);
  REXLOG_API void initialize_logger(rsl::shared_ptr<Logger> logger);

  // Return an existing Logger or nullptr if a Logger with such name doesn't
  // exist.
  // example: rexlog::get("my_logger")->info("hello {}", "world");
  REXLOG_API rsl::shared_ptr<Logger> get(const rex::DebugString& name);

  // Set global formatter. Each sink in each Logger will get a clone of this object
  REXLOG_API void set_formatter(rsl::unique_ptr<rexlog::formatter> formatter);

  // Set global format string.
  // example: rexlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
  REXLOG_API void set_pattern(const rex::DebugString& pattern, PatternTimeType timeType = PatternTimeType::Local);

  // enable global backtrace support
  REXLOG_API void enable_backtrace(size_t nMessages);

  // disable global backtrace support
  REXLOG_API void disable_backtrace();

  // call dump backtrace on default Logger
  REXLOG_API void dump_backtrace();

  // Get global logging level
  REXLOG_API level::LevelEnum get_level();

  // Set global logging level
  REXLOG_API void set_level(level::LevelEnum logLevel);

  // Determine whether the default Logger should log messages with a certain level
  REXLOG_API bool should_log(level::LevelEnum logLevel);

  // Set global flush level
  REXLOG_API void flush_on(level::LevelEnum logLevel);

  // Start/Restart a periodic flusher thread
  // Warning: Use only if all your loggers are thread safe!
  template <typename Rep, typename Period>
  inline void flush_every(rsl::chrono::duration<Rep, Period> interval)
  {
    details::Registry::instance().flush_every(interval);
  }

  // Set global error handler
  REXLOG_API void set_error_handler(void (*handler)(const rex::DebugString& msg));

  // Register the given Logger with the given name
  REXLOG_API void register_logger(rsl::shared_ptr<Logger> logger);

  // Apply a user defined function on all registered loggers
  // Example:
  // rexlog::apply_all([&](rsl::shared_ptr<rexlog::Logger> l) {l->flush();});
  REXLOG_API void apply_all(const rsl::function<void(rsl::shared_ptr<Logger>)>& fun);

  // Drop the reference to the given Logger
  REXLOG_API void drop(const rex::DebugString& name);

  // Drop all references from the Registry
  REXLOG_API void drop_all();

  // stop any running threads started by rexlog and clean Registry loggers
  REXLOG_API void shutdown();

  // Automatic registration of loggers when using rexlog::create() or rexlog::create_async
  REXLOG_API void set_automatic_registration(bool automaticRegistration);

  // API for using default Logger (stdout_color_mt),
  // e.g: rexlog::info("Message {}", 1);
  //
  // The default Logger object can be accessed using the rexlog::default_logger():
  // For example, to add another sink to it:
  // rexlog::default_logger()->sinks().push_back(some_sink);
  //
  // The default Logger can replaced using rexlog::set_default_logger(new_logger).
  // For example, to replace it with a file Logger.
  //
  // IMPORTANT:
  // The default API is thread safe (for _mt loggers), but:
  // set_default_logger() *should not* be used concurrently with the default API.
  // e.g do not call set_default_logger() from one thread while calling rexlog::info() from another.

  REXLOG_API rsl::shared_ptr<rexlog::Logger> default_logger();

  REXLOG_API rexlog::Logger* default_logger_raw();

  REXLOG_API void set_default_logger(rsl::shared_ptr<rexlog::Logger> defaultLogger);

  // Initialize Logger level based on environment configs.
  //
  // Useful for applying REXLOG_LEVEL to manually created loggers.
  //
  // Example:
  //   auto mylogger = rsl::make_shared<rexlog::Logger>("mylogger", ...);
  //   rexlog::apply_logger_env_levels(mylogger);
  REXLOG_API void apply_logger_env_levels(rsl::shared_ptr<Logger> logger);

  template <typename... Args>
  inline void log(SourceLoc source, level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->log(source, lvl, fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void log(level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->log(source_loc {}, lvl, fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void trace(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->trace(fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void debug(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->debug(fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void info(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->info(fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void warn(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->warn(fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void error(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->error(fmt, rsl::forward<Args>(args)...);
  }

  template <typename... Args>
  inline void critical(format_string_t<Args...> fmt, Args&&... args)
  {
    default_logger_raw()->critical(fmt, rsl::forward<Args>(args)...);
  }

  template <typename T>
  inline void log(SourceLoc source, level::LevelEnum lvl, const T& msg)
  {
    default_logger_raw()->log(source, lvl, msg);
  }

  template <typename T>
  inline void log(level::LevelEnum lvl, const T& msg)
  {
    default_logger_raw()->log(lvl, msg);
  }

  template <typename T>
  inline void trace(const T& msg)
  {
    default_logger_raw()->trace(msg);
  }

  template <typename T>
  inline void debug(const T& msg)
  {
    default_logger_raw()->debug(msg);
  }

  template <typename T>
  inline void info(const T& msg)
  {
    default_logger_raw()->info(msg);
  }

  template <typename T>
  inline void warn(const T& msg)
  {
    default_logger_raw()->warn(msg);
  }

  template <typename T>
  inline void error(const T& msg)
  {
    default_logger_raw()->error(msg);
  }

  template <typename T>
  inline void critical(const T& msg)
  {
    default_logger_raw()->critical(msg);
  }

} // namespace rexlog

//
// enable/disable log calls at compile time according to global level.
//
// define REXLOG_ACTIVE_LEVEL to one of those (before including rexlog.h):
// REXLOG_LEVEL_TRACE,
// REXLOG_LEVEL_DEBUG,
// REXLOG_LEVEL_INFO,
// REXLOG_LEVEL_WARN,
// REXLOG_LEVEL_ERROR,
// REXLOG_LEVEL_CRITICAL,
// REXLOG_LEVEL_OFF
//

#ifndef REXLOG_NO_SOURCE_LOC
  #define REXLOG_LOGGER_CALL(Logger, level, ...) (Logger)->log(rexlog::source_loc {__FILE__, __LINE__, REXLOG_FUNCTION}, level, __VA_ARGS__)
#else
  #define REXLOG_LOGGER_CALL(Logger, level, ...) (Logger)->log(rexlog::source_loc {}, level, __VA_ARGS__)
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_TRACE
  #define REXLOG_LOGGER_TRACE(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::trace, __VA_ARGS__)
  #define REXLOG_TRACE(...)                REXLOG_LOGGER_TRACE(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_TRACE(Logger, ...) (void)0
  #define REXLOG_TRACE(...)                (void)0
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_DEBUG
  #define REXLOG_LOGGER_DEBUG(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::debug, __VA_ARGS__)
  #define REXLOG_DEBUG(...)                REXLOG_LOGGER_DEBUG(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_DEBUG(Logger, ...) (void)0
  #define REXLOG_DEBUG(...)                (void)0
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_INFO
  #define REXLOG_LOGGER_INFO(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::info, __VA_ARGS__)
  #define REXLOG_INFO(...)                REXLOG_LOGGER_INFO(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_INFO(Logger, ...) (void)0
  #define REXLOG_INFO(...)                (void)0
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_WARN
  #define REXLOG_LOGGER_WARN(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::warn, __VA_ARGS__)
  #define REXLOG_WARN(...)                REXLOG_LOGGER_WARN(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_WARN(Logger, ...) (void)0
  #define REXLOG_WARN(...)                (void)0
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_ERROR
  #define REXLOG_LOGGER_ERROR(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::err, __VA_ARGS__)
  #define REXLOG_ERROR(...)                REXLOG_LOGGER_ERROR(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_ERROR(Logger, ...) (void)0
  #define REXLOG_ERROR(...)                (void)0
#endif

#if REXLOG_ACTIVE_LEVEL <= REXLOG_LEVEL_CRITICAL
  #define REXLOG_LOGGER_CRITICAL(Logger, ...) REXLOG_LOGGER_CALL(Logger, rexlog::level::critical, __VA_ARGS__)
  #define REXLOG_CRITICAL(...)                REXLOG_LOGGER_CRITICAL(rexlog::default_logger_raw(), __VA_ARGS__)
#else
  #define REXLOG_LOGGER_CRITICAL(Logger, ...) (void)0
  #define REXLOG_CRITICAL(...)                (void)0
#endif

#endif // REXLOG_H
