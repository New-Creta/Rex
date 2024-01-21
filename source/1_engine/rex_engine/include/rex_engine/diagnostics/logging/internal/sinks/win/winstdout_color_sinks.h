

#pragma once

#include "rex_engine/diagnostics/logging/internal/sinks/win/wincolor_sink.h"

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      using StdoutColorSinkMt = WinColorStdoutSinkMt;
      using StdoutColorSinkSt = WinColorStdoutSinkSt;
      using StderrColorSinkMt = WinColorStderrSinkMt;
      using StderrColorSinkSt = WinColorStderrSinkSt;
    } // namespace sinks
  }   // namespace log
} // namespace rex