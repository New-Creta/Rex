

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(const LogMsg& orig_msg)
        : LogMsg {orig_msg}
    {
      buffer.append(logger_name.begin(), logger_name.end());
      buffer.append(payload.begin(), payload.end());
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(const LogMsgBuffer& other)
        : LogMsg {other}
    {
      buffer.append(logger_name.begin(), logger_name.end());
      buffer.append(payload.begin(), payload.end());
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(LogMsgBuffer&& other) REXLOG_NOEXCEPT : LogMsg {other}, buffer {rsl::move(other.buffer)}
    {
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer& LogMsgBuffer::operator=(const LogMsgBuffer& other)
    {
      LogMsg::operator=(other);
      buffer.clear();
      buffer.append(other.buffer.data(), other.buffer.data() + other.buffer.size());
      update_string_views();
      return *this;
    }

    REXLOG_INLINE LogMsgBuffer& LogMsgBuffer::operator=(LogMsgBuffer&& other) REXLOG_NOEXCEPT
    {
      LogMsg::operator=(other);
      buffer = rsl::move(other.buffer);
      update_string_views();
      return *this;
    }

    REXLOG_INLINE void LogMsgBuffer::update_string_views()
    {
      logger_name = string_view_t {buffer.data(), logger_name.size()};
      payload     = string_view_t {buffer.data() + logger_name.size(), payload.size()};
    }

  } // namespace details
} // namespace rexlog
