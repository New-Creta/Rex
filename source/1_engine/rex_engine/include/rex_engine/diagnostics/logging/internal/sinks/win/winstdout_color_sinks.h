

#pragma once

#include "rex_engine/diagnostics/logging/internal/sinks/win/wincolor_sink.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"

namespace rexlog
{
  namespace sinks
  {
      using stdout_color_sink_mt = wincolor_stdout_sink_mt;
      using stdout_color_sink_st = wincolor_stdout_sink_st;
      using stderr_color_sink_mt = wincolor_stderr_sink_mt;
      using stderr_color_sink_st = wincolor_stderr_sink_st;
  } // namespace sinks

  template <typename Factory>
  rsl::shared_ptr<Logger> stdout_color_mt(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic)
  {
      return Factory::template create<sinks::stdout_color_sink_mt>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
  rsl::shared_ptr<Logger> stdout_color_st(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic)
  {
      return Factory::template create<sinks::stdout_color_sink_st>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
  rsl::shared_ptr<Logger> stderr_color_mt(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic)
  {
      return Factory::template create<sinks::stderr_color_sink_mt>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
  rsl::shared_ptr<Logger> stderr_color_st(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic)
  {
      return Factory::template create<sinks::stderr_color_sink_st>(rex::DebugString(loggerName), mode);
  }
} // namespace rexlog