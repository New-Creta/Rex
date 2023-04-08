

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include <algorithm>
#include <iterator>

namespace rexlog
{
  namespace level
  {

#if __cplusplus >= 201703L
    constexpr
#endif
        static string_view_t level_string_views[] REXLOG_LEVEL_NAMES; // NOLINT(modernize-avoid-c-arrays)

    static const char* g_short_level_names[] REXLOG_SHORT_LEVEL_NAMES; // NOLINT(modernize-avoid-c-arrays, cppcoreguidelines-avoid-non-const-global-variables)

    REXLOG_INLINE const string_view_t& to_string_view(rexlog::level::LevelEnum l) REXLOG_NOEXCEPT
    {
      return level_string_views[l];
    }

    REXLOG_INLINE const char* to_short_c_str(rexlog::level::LevelEnum l) REXLOG_NOEXCEPT
    {
      return g_short_level_names[l];
    }

    REXLOG_INLINE rexlog::level::LevelEnum from_str(const rex::DebugString& name) REXLOG_NOEXCEPT
    {
      const auto *it = rsl::find(rsl::begin(level_string_views), rsl::end(level_string_views), name);
      if(it != rsl::end(level_string_views))
        return static_cast<level::LevelEnum>(rsl::distance(rsl::begin(level_string_views), it));

      // check also for "warn" and "err" before giving up..
      if(name == "warn")
      {
        return level::Warn;
      }
      if(name == "err")
      {
        return level::Err;
      }
      return level::Off;
    }
  } // namespace level

  REXLOG_INLINE RexlogEx::RexlogEx(rex::DebugString msg)
      : m_msg(rsl::move(msg))
  {
  }

  REXLOG_INLINE RexlogEx::RexlogEx(const rex::DebugString& msg, int lastErrno)
  {
    m_msg = rex::DebugString(rsl::format("{} : {}", msg, lastErrno));
  }

  REXLOG_INLINE const char* RexlogEx::what() const REXLOG_NOEXCEPT
  {
    return m_msg.c_str();
  }

  REXLOG_INLINE void throw_rexlog_ex(const rex::DebugString& msg, int lastErrno)
  {
    REXLOG_THROW(RexlogEx(msg, lastErrno));
  }

  REXLOG_INLINE void throw_rexlog_ex(const char* msg, int lastErrno)
  {
    throw_rexlog_ex(rex::DebugString(msg), lastErrno);
  }

  REXLOG_INLINE void throw_rexlog_ex(rex::DebugString msg)
  {
    REXLOG_THROW(RexlogEx(rsl::move(msg)));
  }

  REXLOG_INLINE void throw_rexlog_ex(const char* msg)
  {
    throw_rexlog_ex(rex::DebugString(msg));
  }

} // namespace rexlog
