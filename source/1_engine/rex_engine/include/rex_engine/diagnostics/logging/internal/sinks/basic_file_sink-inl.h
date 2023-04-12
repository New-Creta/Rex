

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace sinks
  {

    template <typename Mutex>
    REXLOG_INLINE BasicFileSink<Mutex>::BasicFileSink(const filename_t& filename, bool truncate, const FileEventHandlers& eventHandlers)
        : m_file_helper {eventHandlers}
    {
      m_file_helper.open(filename, truncate);
    }

    template <typename Mutex>
    REXLOG_INLINE const filename_t& BasicFileSink<Mutex>::filename() const
    {
      return m_file_helper.filename();
    }

    template <typename Mutex>
    REXLOG_INLINE void BasicFileSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
    {
      memory_buf_t formatted;
      BaseSink<Mutex>::formatter()->format(msg, formatted);
      m_file_helper.write(formatted);
    }

    template <typename Mutex>
    REXLOG_INLINE void BasicFileSink<Mutex>::flush_impl()
    {
      m_file_helper.flush();
    }

  } // namespace sinks
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
