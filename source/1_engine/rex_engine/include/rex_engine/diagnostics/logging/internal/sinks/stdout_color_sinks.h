

#pragma once

#ifdef _WIN32
  #include "rex_engine/diagnostics/logging/internal/sinks/wincolor_sink.h"
#else
  #include "rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink.h"
#endif

#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"

namespace rexlog
{
  namespace sinks
  {
#ifdef _WIN32
    using stdout_color_sink_mt = wincolor_stdout_sink_mt;
    using stdout_color_sink_st = wincolor_stdout_sink_st;
    using stderr_color_sink_mt = wincolor_stderr_sink_mt;
    using stderr_color_sink_st = wincolor_stderr_sink_st;
#else
    using stdout_color_sink_mt = ansicolor_stdout_sink_mt;
    using stdout_color_sink_st = ansicolor_stdout_sink_st;
    using stderr_color_sink_mt = ansicolor_stderr_sink_mt;
    using stderr_color_sink_st = ansicolor_stderr_sink_st;
#endif
  } // namespace sinks

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stdout_color_mt(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stdout_color_st(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stderr_color_mt(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic);

  template <typename Factory = rexlog::SynchronousFactory>
  rsl::shared_ptr<Logger> stderr_color_st(const rex::DebugString& loggerName, ColorMode mode = ColorMode::Automatic);

} // namespace rexlog