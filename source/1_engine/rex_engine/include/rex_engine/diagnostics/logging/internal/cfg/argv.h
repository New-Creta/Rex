

#pragma once
#include "rex_engine/diagnostics/logging/internal/cfg/helpers.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"

//
// Init log levels using each argv entry that starts with "REXLOG_LEVEL="
//
// set all loggers to debug level:
// example.exe "REXLOG_LEVEL=debug"

// set logger1 to trace level
// example.exe "REXLOG_LEVEL=logger1=trace"

// turn off all logging except for logger1 and logger2:
// example.exe "REXLOG_LEVEL=off,logger1=debug,logger2=info"

namespace rexlog
{
  namespace cfg
  {

    // search for REXLOG_LEVEL= in the args and use it to init the levels
    inline void load_argv_levels(int argc, const char** argv)
    {
      const rex::DebugString rexlog_level_prefix("REXLOG_LEVEL=");
      for(int i = 1; i < argc; i++)
      {
        rex::DebugString arg(argv[i]);
        if(arg.find(rexlog_level_prefix) == 0)
        {
          auto levels_string = arg.substr(rexlog_level_prefix.size());
          helpers::load_levels(rex::DebugString(levels_string));
        }
      }
    }

    inline void load_argv_levels(int argc, char** argv)
    {
      load_argv_levels(argc, const_cast<const char**>(argv));
    }

  } // namespace cfg
} // namespace rexlog