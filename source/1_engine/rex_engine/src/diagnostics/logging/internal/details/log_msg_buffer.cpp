#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"

#include "rex_std/bonus/string.h"

// IWYU pragma: no_include <built-in>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace details
    {
      //-------------------------------------------------------------------------
      LogMsgBuffer::LogMsgBuffer(const LogMsgBuffer& other)
          : LogMsg {other}
      {
        m_buffer.append(logger_name().begin(), logger_name().end());
        m_buffer.append(payload().begin(), payload().end());
        update_string_views();
      }

      //-------------------------------------------------------------------------
      LogMsgBuffer::LogMsgBuffer(LogMsgBuffer&& other) noexcept
          : LogMsg {other}
          , m_buffer {rsl::move(other.m_buffer)}
      {
        update_string_views();
      }

      //-------------------------------------------------------------------------
      LogMsgBuffer::LogMsgBuffer(const LogMsg& origMsg)
          : LogMsg {origMsg}
      {
        m_buffer.append(logger_name().begin(), logger_name().end());
        m_buffer.append(payload().begin(), payload().end());
        update_string_views();
      }

      //-------------------------------------------------------------------------
      LogMsgBuffer& LogMsgBuffer::operator=(const LogMsgBuffer& other)
      {
        LogMsg::operator=(other);
        m_buffer.clear();
        m_buffer.append(other.m_buffer.data(), other.m_buffer.data() + other.m_buffer.size());
        update_string_views();
        return *this;
      }

      //-------------------------------------------------------------------------
      LogMsgBuffer& LogMsgBuffer::operator=(LogMsgBuffer&& other) noexcept
      {
        LogMsg::operator=(other);
        m_buffer = rsl::move(other.m_buffer);
        update_string_views();
        return *this;
      }

      //-------------------------------------------------------------------------
      void LogMsgBuffer::update_string_views()
      {
        set_logger_name(rsl::string_view {m_buffer.data(), logger_name().size()});
        set_payload(rsl::string_view {m_buffer.data() + logger_name().size(), payload().size()});
      }
    } // namespace details
  }   // namespace log
} // namespace rex

// NOLINTEND(misc-definitions-in-headers)
