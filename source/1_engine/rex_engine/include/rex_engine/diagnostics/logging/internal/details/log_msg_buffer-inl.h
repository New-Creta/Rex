

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(const LogMsg& origMsg)
        : LogMsg {origMsg}
    {
      m_buffer.append(logger_name.begin(), logger_name.end());
      m_buffer.append(payload.begin(), payload.end());
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(const LogMsgBuffer& other)
        : LogMsg {other}
    {
      m_buffer.append(logger_name.begin(), logger_name.end());
      m_buffer.append(payload.begin(), payload.end());
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer::LogMsgBuffer(LogMsgBuffer&& other) REXLOG_NOEXCEPT 
      : LogMsg {other}
      , m_buffer {rsl::move(other.m_buffer)}
    {
      update_string_views();
    }

    REXLOG_INLINE LogMsgBuffer& LogMsgBuffer::operator=(const LogMsgBuffer& other)
    {
      LogMsg::operator=(other);
      m_buffer.clear();
      m_buffer.append(other.m_buffer.data(), other.m_buffer.data() + other.m_buffer.size());
      update_string_views();
      return *this;
    }

    REXLOG_INLINE LogMsgBuffer& LogMsgBuffer::operator=(LogMsgBuffer&& other) REXLOG_NOEXCEPT
    {
      LogMsg::operator=(other);
      m_buffer = rsl::move(other.m_buffer);
      update_string_views();
      return *this;
    }

    REXLOG_INLINE void LogMsgBuffer::update_string_views()
    {
      logger_name = string_view_t {m_buffer.data(), logger_name.size()};
      payload     = string_view_t {m_buffer.data() + logger_name.size(), payload.size()};
    }

  } // namespace details
} // namespace rexlog
