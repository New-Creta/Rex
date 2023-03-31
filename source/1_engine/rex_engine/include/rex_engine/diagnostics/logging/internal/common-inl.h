

#pragma once

#include <algorithm>
#include <iterator>
#include <rex_engine/diagnostics/logging/internal/common.h>

namespace rexlog
{
  namespace level
  {

#if __cplusplus >= 201703L
    constexpr
#endif
        static string_view_t level_string_views[] REXLOG_LEVEL_NAMES;

    static const char* short_level_names[] REXLOG_SHORT_LEVEL_NAMES;

    REXLOG_INLINE const string_view_t& to_string_view(rexlog::level::level_enum l) REXLOG_NOEXCEPT
    {
      return level_string_views[l];
    }

    REXLOG_INLINE const char* to_short_c_str(rexlog::level::level_enum l) REXLOG_NOEXCEPT
    {
      return short_level_names[l];
    }

    REXLOG_INLINE rexlog::level::level_enum from_str(const rsl::string& name) REXLOG_NOEXCEPT
    {
      auto it = rsl::find(rsl::begin(level_string_views), rsl::end(level_string_views), name);
      if(it != rsl::end(level_string_views))
        return static_cast<level::level_enum>(rsl::distance(rsl::begin(level_string_views), it));

      // check also for "warn" and "err" before giving up..
      if(name == "warn")
      {
        return level::warn;
      }
      if(name == "err")
      {
        return level::err;
      }
      return level::off;
    }
  } // namespace level

  REXLOG_INLINE rexlog_ex::rexlog_ex(rsl::string msg)
      : msg_(rsl::move(msg))
  {
  }

  REXLOG_INLINE rexlog_ex::rexlog_ex(const rsl::string& msg, int last_errno)
  {
    msg_ = rsl::string(rsl::format("{} : {}", msg, last_errno));
  }

  REXLOG_INLINE const char* rexlog_ex::what() const REXLOG_NOEXCEPT
  {
    return msg_.c_str();
  }

  REXLOG_INLINE void throw_rexlog_ex(const rsl::string& msg, int last_errno)
  {
    REXLOG_THROW(rexlog_ex(msg, last_errno));
  }

  REXLOG_INLINE void throw_rexlog_ex(const char* msg, int last_errno)
  {
    throw_rexlog_ex(rsl::string(msg), last_errno);
  }

  REXLOG_INLINE void throw_rexlog_ex(rsl::string msg)
  {
    REXLOG_THROW(rexlog_ex(rsl::move(msg)));
  }

  REXLOG_INLINE void throw_rexlog_ex(const char* msg)
  {
    throw_rexlog_ex(rsl::string(msg));
  }

} // namespace rexlog
