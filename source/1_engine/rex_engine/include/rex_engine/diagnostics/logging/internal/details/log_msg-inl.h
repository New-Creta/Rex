

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE inline LogMsg::LogMsg(rexlog::log_clock::time_point logTime, rexlog::SourceLoc loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
        : logger_name(aLoggerName)
        , level(lvl)
        , time(logTime)
#ifndef REXLOG_NO_THREAD_ID
        , thread_id(os::thread_id())
#endif
        , source(loc)
        , payload(msg)
    {
    }

    REXLOG_INLINE inline LogMsg::LogMsg(rexlog::SourceLoc loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
        : LogMsg(os::now(), loc, aLoggerName, lvl, msg)
    {
    }

    REXLOG_INLINE inline LogMsg::LogMsg(string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
        : LogMsg(os::now(), SourceLoc {}, aLoggerName, lvl, msg)
    {
    }

  } // namespace details
} // namespace rexlog
