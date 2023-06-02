#include "rex_engine/diagnostics/logging/logger_config.h"

#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/algorithm.h"
#include "rex_std/ctype.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/sstream.h"
#include "rex_std/string.h"
#include "rex_std/utility.h"

#include <algorithm>

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
      rsl::vector<rsl::string_view> split(rsl::string_view str, rsl::string_view delim)
      {
        rsl::vector<rsl::string_view> result;

        count_t start = 0;
        count_t end   = 0;
        for(count_t i = 0; i < str.length(); ++i)
        {
          auto it = rsl::find(delim.cbegin(), delim.cend(), str[i]);
          if(it != delim.cend())
          {
            end = i;

            if(start == end)
            {
              ++start;
              continue;
            }

            const rsl::string_view value = str.substr(start, end - start);
            result.push_back(value);
            start = end + 1;
          }
        }

        if(start != end)
        {
          const rsl::string_view value = str.substr(start, end - start);
          result.push_back(value);
        }

        return result;
      }

      // return vector of key/value pairs from sequence of "K1=V1,K2=V2,.."
      // "a=AAA,b=BBB,c=CCC,.." => {("a","AAA"),("b","BBB"),("c", "CCC"),...}
      rsl::unordered_map<rsl::string_view, rsl::string_view> extract_key_vals(rsl::string_view str)
      {
        rsl::unordered_map<rsl::string_view, rsl::string_view> rv {};
        const rsl::vector<rsl::string_view> key_value_pairs = split(str, ",");

        for(const rsl::string_view key_value_pair: key_value_pairs)
        {
          rsl::vector<rsl::string_view> key_value = split(key_value_pair, "=");

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
        rsl::unordered_map<rsl::string_view, rexlog::level::LevelEnum> levels;
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
    void init()
    {
      rsl::optional<rsl::string_view> log_level = cmdline::get_argument("LogLevel");

      if(log_level)
      {
        helpers::load_levels(log_level.value());
      }
    }
  } // namespace diagnostics
} // namespace rex