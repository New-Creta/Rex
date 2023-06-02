#pragma once

#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/unix/ansicolor_sink.h"

namespace rexlog
{
  namespace sinks
  {
    using stdout_color_sink_mt = ansicolor_stdout_sink_mt;
    using stdout_color_sink_st = ansicolor_stdout_sink_st;
    using stderr_color_sink_mt = ansicolor_stderr_sink_mt;
    using stderr_color_sink_st = ansicolor_stderr_sink_st;
  } // namespace sinks
} // namespace rexlog