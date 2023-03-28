

#pragma once

#include <rexlog/cfg/helpers.h>

#include <rexlog/rexlog.h>
#include <rexlog/details/os.h>
#include <rexlog/details/registry.h>

#include "rex_std/algorithm.h"
#include "rex_std/string.h"
#include "rex_std/utility.h"
#include "rex_std/sstream.h"

#include "rex_std/internal/utility/pair.h"

namespace rexlog {
namespace cfg {
namespace helpers {

// inplace convert to lowercase
inline rsl::string &to_lower_(rsl::string &str)
{
    rsl::transform(
        str.begin(), str.end(), str.begin(), [](char ch) { return static_cast<char>((ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch); });
    return str;
}

// inplace trim spaces
inline rsl::string &trim_(rsl::string &str)
{
    const char *spaces = " \n\r\t";
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

inline rsl::pair<rsl::string, rsl::string> extract_kv_(char sep, const rsl::string &str)
{
    auto n = str.find(sep);
    rsl::string k, v;
    if (n == rsl::string::npos())
    {
        v = str;
    }
    else
    {
        k = rsl::string(str.substr(0, n));
        v = rsl::string(str.substr(n + 1));
    }
    return rsl::make_pair(trim_(k), trim_(v));
}

// return vector of key/value pairs from sequence of "K1=V1,K2=V2,.."
// "a=AAA,b=BBB,c=CCC,.." => {("a","AAA"),("b","BBB"),("c", "CCC"),...}
inline rsl::unordered_map<rsl::string, rsl::string> extract_key_vals_(const rsl::string &str)
{
    rsl::string token;
    rsl::istringstream token_stream(str);
    rsl::unordered_map<rsl::string, rsl::string> rv{};
    while (rsl::getline(token_stream, token, ','))
    {
        if (token.empty())
        {
            continue;
        }
        auto kv = extract_kv_('=', token);
        rv[kv.first] = kv.second;
    }
    return rv;
}

REXLOG_INLINE void load_levels(const rsl::string &input)
{
    if (input.empty() || input.size() > 512)
    {
        return;
    }

    auto key_vals = extract_key_vals_(input);
    rsl::unordered_map<rsl::string, level::level_enum> levels;
    level::level_enum global_level = level::info;
    bool global_level_found = false;

    for (auto &name_level : key_vals)
    {
        auto &logger_name = name_level.key;
        auto level_name = rsl::string(to_lower_(name_level.value));
        auto level = level::from_str(level_name);
        // ignore unrecognized level names
        if (level == level::off && level_name != "off")
        {
            continue;
        }
        if (logger_name.empty()) // no logger name indicate global level
        {
            global_level_found = true;
            global_level = level;
        }
        else
        {
            levels[logger_name] = level;
        }
    }

    details::registry::instance().set_levels(rsl::move(levels), global_level_found ? &global_level : nullptr);
}

} // namespace helpers
} // namespace cfg
} // namespace rexlog
