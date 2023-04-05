

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include <unordered_map>

namespace rexlog
{
  namespace cfg
  {
    namespace helpers
    {
      //
      // Init levels from given string
      //
      // Examples:
      //
      // set global level to debug: "debug"
      // turn off all logging except for logger1: "off,logger1=debug"
      // turn off all logging except for logger1 and logger2: "off,logger1=debug,logger2=info"
      //
      REXLOG_API void load_levels(const rex::DebugString& txt);
    } // namespace helpers

  } // namespace cfg
} // namespace rexlog
