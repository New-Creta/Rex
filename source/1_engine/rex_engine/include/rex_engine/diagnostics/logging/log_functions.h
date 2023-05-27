#pragma once

#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/diagnostics/logging/log_category.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_std/memory.h"

namespace rex
{
  rexlog::Logger& get_logger(const LogCategory& category);

  template <typename T>
  void trace_fatal_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).critical(msg);
  }
  template <typename T>
  void trace_error_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).error(msg);
  }
  template <typename T>
  void trace_warning_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).warn(msg);
  }
  template <typename T>
  void trace_log_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).info(msg);
  }
  template <typename T>
  void trace_verbose_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).debug(msg);
  }
  template <typename T>
  void trace_very_verbose_log(const LogCategory& category, const T& msg)
  {
    get_logger(category).trace(msg);
  }

  template <typename FormatString, typename... Args>
  void trace_fatal_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).critical(fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_error_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).error(fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_warning_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).warn(fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_log_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).info(fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_verbose_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).debug(fmt, rsl::forward<Args>(args)...);
  }
  template <typename FormatString, typename... Args>
  void trace_very_verbose_log(const LogCategory& category, const FormatString& fmt, Args&&... args)
  {
    get_logger(category).trace(fmt, rsl::forward<Args>(args)...);
  }

  template <typename T>
  void trace_log(const LogCategory& category, LogVerbosity verbosity, const T& msg)
  {
    if(category.get_verbosity() < verbosity)
    {
      // Too low of a log level to print log statements
      return;
    }

    switch(verbosity & LogVerbosity::VerbosityMask)
    {
      case LogVerbosity::Fatal: trace_fatal_log(category, msg); break;
      case LogVerbosity::Error: trace_error_log(category, msg); break;
      case LogVerbosity::Warning: trace_warning_log(category, msg); break;
      case LogVerbosity::Log: trace_log_log(category, msg); break;
      case LogVerbosity::Verbose: trace_verbose_log(category, msg); break;
      case LogVerbosity::VeryVerbose: trace_very_verbose_log(category, msg); break;
      default: trace_fatal_log(category, "Unknown log category: {0}", category.get_category_name()); break;
    }
  }

  template <typename FormatString, typename... Args>
  void trace_log(const LogCategory& category, LogVerbosity verbosity, const FormatString& fmt, Args&&... args)
  {
    if(category.get_verbosity() < verbosity)
    {
      // Too low of a log level to print log statements
      return;
    }

    switch(verbosity & LogVerbosity::VerbosityMask)
    {
      case LogVerbosity::Fatal: trace_fatal_log(category, fmt, rsl::forward<Args>(args)...); break;
      case LogVerbosity::Error: trace_error_log(category, fmt, rsl::forward<Args>(args)...); break;
      case LogVerbosity::Warning: trace_warning_log(category, fmt, rsl::forward<Args>(args)...); break;
      case LogVerbosity::Log: trace_log_log(category, fmt, rsl::forward<Args>(args)...); break;
      case LogVerbosity::Verbose: trace_verbose_log(category, fmt, rsl::forward<Args>(args)...); break;
      case LogVerbosity::VeryVerbose: trace_very_verbose_log(category, fmt, rsl::forward<Args>(args)...); break;
      default: trace_fatal_log(category, "Unknown log category: {0}", category.get_category_name()); break;
    }
  }
} // namespace rex