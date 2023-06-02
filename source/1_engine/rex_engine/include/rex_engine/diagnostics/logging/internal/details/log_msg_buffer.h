

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"

namespace rexlog
{
  namespace details
  {
    // Extend LogMsg with internal buffer to store its payload.
    // This is needed since LogMsg holds string_views that points to stack data.

    class LogMsgBuffer : public LogMsg
    {
    public:
      LogMsgBuffer() = default;
      LogMsgBuffer(const LogMsgBuffer& other);
      LogMsgBuffer(LogMsgBuffer&& other) noexcept;

      ~LogMsgBuffer() override = default;

      explicit LogMsgBuffer(const LogMsg& origMsg);

      LogMsgBuffer& operator=(const LogMsgBuffer& other);
      LogMsgBuffer& operator=(LogMsgBuffer&& other) noexcept;

    private:
      void update_string_views();

      memory_buf_t m_buffer;
    };
  } // namespace details
} // namespace rexlog
