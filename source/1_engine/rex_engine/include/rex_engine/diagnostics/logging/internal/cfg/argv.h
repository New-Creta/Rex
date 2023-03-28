

#pragma once
#include <rexlog/cfg/helpers.h>
#include <rexlog/details/registry.h>

//
// Init log levels using each argv entry that starts with "REXLOG_LEVEL="
//
// set all loggers to debug level:
// example.exe "REXLOG_LEVEL=debug"

// set logger1 to trace level
// example.exe "REXLOG_LEVEL=logger1=trace"

// turn off all logging except for logger1 and logger2:
// example.exe "REXLOG_LEVEL=off,logger1=debug,logger2=info"

namespace rexlog {
namespace cfg {

// search for REXLOG_LEVEL= in the args and use it to init the levels
inline void load_argv_levels(int argc, const char **argv)
{
    const rsl::string rexlog_level_prefix = "REXLOG_LEVEL=";
    for (int i = 1; i < argc; i++)
    {
        rsl::string arg = argv[i];
        if (arg.find(rexlog_level_prefix) == 0)
        {
            auto levels_string = arg.substr(rexlog_level_prefix.size());
            helpers::load_levels(levels_string);
        }
    }
}

inline void load_argv_levels(int argc, char **argv)
{
    load_argv_levels(argc, const_cast<const char **>(argv));
}

} // namespace cfg
} // namespace rexlog
