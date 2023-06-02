#include "rex_engine/diagnostics/logging/logger_config.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/algorithm.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/sstream.h"
#include "rex_std/string.h"
#include "rex_std/utility.h"

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
      // inplace convert to lowercase
      inline rex::DebugString& to_lower_impl(rex::DebugString& str)
      {
        rsl::transform(str.begin(), str.end(), str.begin(), [](char ch) { return static_cast<char>((ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch); });
        return str;
      }

      // inplace trim spaces
      inline rex::DebugString& trim_impl(rex::DebugString& str)
      {
        const char* spaces = " \n\r\t";
        str.erase(str.find_last_not_of(rsl::string_view(spaces)) + 1);
        str.erase(0, str.find_first_not_of(rsl::string_view(spaces)));
        return str;
      }

      // return (name,value) trimmed pair from given "name=value" string.
      // return empty string on missing parts
      // "key=val" => ("key", "val")
      // " key  =  val " => ("key", "val")
      // "key=" => ("key", "")
      // "val" => ("", "val")

      inline rsl::pair<rex::DebugString, rex::DebugString> extract_kv_impl(char sep, const rex::DebugString& str)
      {
        auto n = str.find(sep);
        rex::DebugString k;
        rex::DebugString v;
        if(n == rex::DebugString::npos())
        {
          v = str;
        }
        else
        {
          k = rex::DebugString(str.substr(0, n));
          v = rex::DebugString(str.substr(n + 1));
        }
        return rsl::make_pair(trim_impl(k), trim_impl(v));
      }

      // return vector of key/value pairs from sequence of "K1=V1,K2=V2,.."
      // "a=AAA,b=BBB,c=CCC,.." => {("a","AAA"),("b","BBB"),("c", "CCC"),...}
      inline rex::DebugHashTable<rex::DebugString, rex::DebugString> extract_key_vals_impl(const rex::DebugString& str)
      {
        rex::DebugString token;
        rex::DebugStringStream token_stream(str);
        rex::DebugHashTable<rex::DebugString, rex::DebugString> rv {};
        while(rsl::getline(token_stream, token, ','))
        {
          if(token.empty())
          {
            continue;
          }
          auto kv      = extract_kv_impl('=', token);
          rv[kv.first] = kv.second;
        }
        return rv;
      }

      void load_levels(const rex::DebugString& input)
      {
        if(input.empty() || input.size() > 512)
        {
          return;
        }

        auto key_vals = extract_key_vals_impl(input);
        rex::DebugHashTable<rex::DebugString, rexlog::level::LevelEnum> levels;
        rexlog::level::LevelEnum global_level = rexlog::level::LevelEnum::Info;
        bool global_level_found               = false;

        for(auto& name_level: key_vals)
        {
          const auto& logger_name = name_level.key;
          auto level_name         = to_lower_impl(name_level.value);
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
    void load_log_levels(const rsl::vector<rsl::string_view>& args)
    {
      const rsl::string_view rexlog_level_prefix("REXLOG_LEVEL=");

      for(const rsl::string_view s: args)
      {
        if(s.find(rexlog_level_prefix) == 0)
        {
          auto levels_string = s.substr(rexlog_level_prefix.size());
          helpers::load_levels(rex::DebugString(levels_string));
        }
      }
    }
  } // namespace diagnostics
} // namespace rex