

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"

namespace rexlog
{
  namespace details
  {

    // Extend LogMsg with internal buffer to store its payload.
    // This is needed since LogMsg holds string_views that points to stack data.

    class REXLOG_API LogMsgBuffer : public LogMsg
    {
      memory_buf_t m_buffer;
      void update_string_views();

    public:
      LogMsgBuffer() = default;
      explicit LogMsgBuffer(const LogMsg& origMsg);
      LogMsgBuffer(const LogMsgBuffer& other);
      LogMsgBuffer(LogMsgBuffer&& other) REXLOG_NOEXCEPT;
      ~LogMsgBuffer() = default;
      LogMsgBuffer& operator=(const LogMsgBuffer& other);
      LogMsgBuffer& operator=(LogMsgBuffer&& other) REXLOG_NOEXCEPT;
    };

  } // namespace details
} // namespace rexlog
