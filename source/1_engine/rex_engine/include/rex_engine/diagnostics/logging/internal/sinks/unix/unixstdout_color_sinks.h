#pragma once

#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/unix/ansicolor_sink.h"

namespace rexlog
{
  namespace sinks
  {
    using StdoutColorSinkMt = ANSIColorStdoutSinkMt;
    using StdoutColorSinkSt = ANSIColorStdoutSinkSt;
    using StderrColorSinkMt = ANSIColorStderrSinkMt;
    using StderrColorSinkSt = ANSIColorStderrSinkSt;
  } // namespace sinks
} // namespace rexlog