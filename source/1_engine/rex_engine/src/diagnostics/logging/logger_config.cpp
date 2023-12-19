#include "rex_engine/diagnostics/logging/logger_config.h"

#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

//
// Init log levels using each argv entry that starts with "REXLOG_LEVEL="
//
// set all loggers to debug level:
// example.exe "REXLOG_LEVEL=debug"

// set logger1 to trace level
// example.exe "REXLOG_LEVEL=logger1=trace"

// turn off all logging except for logger1 and logger2:
// example.exe "REXLOG_LEVEL=off,logger1=debug,logger2=info"

namespace rex
{
  namespace diagnostics
  {
    namespace helpers
    {
      // return vector of key/value pairs from sequence of "K1=V1,K2=V2,.."
      // "a=AAA,b=BBB,c=CCC,.." => {("a","AAA"),("b","BBB"),("c", "CCC"),...}
      rsl::unordered_map<rsl::string_view, rsl::string_view> extract_key_vals(rsl::string_view str)
      {
        rsl::unordered_map<rsl::string_view, rsl::string_view> rv {};
        const rsl::vector<rsl::string_view> key_value_pairs = rsl::split(str, ",");

        for(const rsl::string_view key_value_pair: key_value_pairs)
        {
          rsl::vector<rsl::string_view> key_value = rsl::split(key_value_pair, "=");

          REX_ASSERT_X(key_value.size() == 2, "Invalid logger level found at {}", key_value_pair);
          rv[key_value[0]] = key_value[1];
        }

        return rv;
      }

      void load_levels(rsl::string_view input)
      {
        if(input.empty() || input.size() > 512)
        {
          return;
        }

        auto key_vals = extract_key_vals(input);
        rexlog::details::Registry::LogLevels levels;
        rexlog::level::LevelEnum global_level = rexlog::level::LevelEnum::Info;
        bool global_level_found               = false;

        for(auto& name_level: key_vals)
        {
          const auto& logger_name = name_level.key;
          auto level_name         = name_level.value;
          auto level              = rexlog::level::from_str(level_name);
          // ignore unrecognized level names
          if(level == rexlog::level::LevelEnum::Off && level_name != "off")
          {
            continue;
          }
          if(logger_name.empty()) // no Logger name indicate global level
          {
            global_level_found = true;
            global_level       = level;
          }
          else
          {
            levels[logger_name] = level;
          }
        }

        rexlog::details::Registry::instance().set_levels(rsl::move(levels), global_level_found ? &global_level : nullptr);
      }

    } // namespace helpers

    // search for REXLOG_LEVEL= in the args and use it to init the levels
    void init_log_levels()
    {
      rsl::optional<rsl::string_view> log_level = cmdline::get_argument("LogLevel");

      if(log_level)
      {
        helpers::load_levels(log_level.value());
      }
    }
  } // namespace diagnostics
} // namespace rex