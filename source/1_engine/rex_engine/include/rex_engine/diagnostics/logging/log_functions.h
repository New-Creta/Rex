#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/log_category.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace log
  {
    void init();
    bool is_supressed(LogVerbosity verbosity);
  }

  rex::log::Logger& get_logger(const LogCategory& category);

  template <typename T>
  void trace_fatal_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Critical, msg);
  }
  template <typename T>
  void trace_error_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Err, msg);
  }
  template <typename T>
  void trace_warning_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Warn, msg);
  }
  template <typename T>
  void trace_log_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Info, msg);
  }
  template <typename T>
  void trace_verbose_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Debug, msg);
  }
  template <typename T>
  void trace_very_verbose_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Trace, msg);
  }

  template <typename FormatString, typename... Args>
  void trace_fatal_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Critical, fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_error_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Err, fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_warning_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Warn, fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_log_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Info, fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_verbose_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Debug, fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_very_verbose_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).log(rex::log::level::LevelEnum::Trace, fmt, rsl::forward<Args>(args)...);
  }

  template <typename T>
  void trace_log(const LogCategory& category, log::LogVerbosity verbosity, const T& msg)
  {
    switch(verbosity)
    {
      case log::LogVerbosity::Critical: trace_fatal_log(category, msg); break;
      case log::LogVerbosity::Err: trace_error_log(category, msg); break;
      case log::LogVerbosity::Warn: trace_warning_log(category, msg); break;
      case log::LogVerbosity::Info: trace_log_log(category, msg); break;
      case log::LogVerbosity::Debug: trace_verbose_log(category, msg); break;
      case log::LogVerbosity::Trace: trace_very_verbose_log(category, msg); break;
      default: trace_fatal_log(category, "Unknown log category: {0}", category.get_category_name()); break;
    }
  }

  template <typename FormatString, typename... Args>
  void trace_log(const LogCategory& category, log::LogVerbosity verbosity, const FormatString& fmt, Args&&... args)
  {
    switch(verbosity)
    {
      case log::LogVerbosity::Critical: trace_fatal_log(category, fmt, rsl::forward<Args>(args)...); break;
      case log::LogVerbosity::Err: trace_error_log(category, fmt, rsl::forward<Args>(args)...); break;
      case log::LogVerbosity::Warn: trace_warning_log(category, fmt, rsl::forward<Args>(args)...); break;
      case log::LogVerbosity::Info: trace_log_log(category, fmt, rsl::forward<Args>(args)...); break;
      case log::LogVerbosity::Debug: trace_verbose_log(category, fmt, rsl::forward<Args>(args)...); break;
      case log::LogVerbosity::Trace: trace_very_verbose_log(category, fmt, rsl::forward<Args>(args)...); break;
      default: trace_fatal_log(category, "Unknown log category: {0}", category.get_category_name()); break;
    }
  }
} // namespace rex