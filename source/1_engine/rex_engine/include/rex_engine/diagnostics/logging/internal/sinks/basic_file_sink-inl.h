

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"

namespace rexlog
{
  namespace sinks
  {

    template <typename Mutex>
    REXLOG_INLINE BasicFileSink<Mutex>::BasicFileSink(const filename_t& filename, bool truncate, const file_event_handlers& event_handlers)
        : m_file_helper {event_handlers}
    {
      m_file_helper.open(filename, truncate);
    }

    template <typename Mutex>
    REXLOG_INLINE const filename_t& BasicFileSink<Mutex>::filename() const
    {
      return m_file_helper.filename();
    }

    template <typename Mutex>
    REXLOG_INLINE void BasicFileSink<Mutex>::sink_it_(const details::LogMsg& msg)
    {
      memory_buf_t formatted;
      BaseSink<Mutex>::m_formatter->format(msg, formatted);
      m_file_helper.write(formatted);
    }

    template <typename Mutex>
    REXLOG_INLINE void BasicFileSink<Mutex>::flush_()
    {
      m_file_helper.flush();
    }

  } // namespace sinks
} // namespace rexlog
