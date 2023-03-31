

#pragma once

#include <rex_engine/diagnostics/logging/internal/details/log_msg.h>
#include <rex_engine/diagnostics/logging/internal/details/os.h>

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE log_msg::log_msg(rexlog::log_clock::time_point log_time, rexlog::source_loc loc, string_view_t a_logger_name, rexlog::level::level_enum lvl, rexlog::string_view_t msg)
        : logger_name(a_logger_name)
        , level(lvl)
        , time(log_time)
#ifndef REXLOG_NO_THREAD_ID
        , thread_id(os::thread_id())
#endif
        , source(loc)
        , payload(msg)
    {
    }

    REXLOG_INLINE log_msg::log_msg(rexlog::source_loc loc, string_view_t a_logger_name, rexlog::level::level_enum lvl, rexlog::string_view_t msg)
        : log_msg(os::now(), loc, a_logger_name, lvl, msg)
    {
    }

    REXLOG_INLINE log_msg::log_msg(string_view_t a_logger_name, rexlog::level::level_enum lvl, rexlog::string_view_t msg)
        : log_msg(os::now(), source_loc {}, a_logger_name, lvl, msg)
    {
    }

  } // namespace details
} // namespace rexlog
