

#pragma once

#include "rex_engine/diagnostics/logging/internal/sinks/win/wincolor_sink.h"

namespace rexlog
{
  namespace sinks
  {
      using stdout_color_sink_mt = wincolor_stdout_sink_mt;
      using stdout_color_sink_st = wincolor_stdout_sink_st;
      using stderr_color_sink_mt = wincolor_stderr_sink_mt;
      using stderr_color_sink_st = wincolor_stderr_sink_st;
  } // namespace sinks
} // namespace rexlog