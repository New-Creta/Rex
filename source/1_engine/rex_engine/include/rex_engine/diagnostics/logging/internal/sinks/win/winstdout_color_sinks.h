

#pragma once

#include "rex_engine/diagnostics/logging/internal/sinks/win/wincolor_sink.h"

namespace rexlog
{
  namespace sinks
  {
    using StdoutColorSinkMt = WinColorStdoutSinkMt;
    using StdoutColorSinkSt = WinColorStdoutSinkSt;
    using StderrColorSinkMt = WinColorStderrSinkMt;
    using StderrColorSinkSt = WinColorStderrSinkSt;
  } // namespace sinks
} // namespace rexlog