

#pragma once
#include "rex_engine/diagnostics/logging/internal/cfg/helpers.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"

//
// Init levels and patterns from env variables REXLOG_LEVEL
// Inspired from Rust's "env_logger" crate (https://crates.io/crates/env_logger).
// Note - fallback to "info" level on unrecognized levels
//
// Examples:
//
// set global level to debug:
// export REXLOG_LEVEL=debug
//
// turn off all logging except for logger1:
// export REXLOG_LEVEL="*=off,logger1=debug"
//

// turn off all logging except for logger1 and logger2:
// export REXLOG_LEVEL="off,logger1=debug,logger2=info"

namespace rexlog
{
  namespace cfg
  {
    inline void load_env_levels()
    {
      auto env_val = details::os::getenv("REXLOG_LEVEL");
      if(!env_val.empty())
      {
        helpers::load_levels(env_val);
      }
    }

  } // namespace cfg
} // namespace rexlog
