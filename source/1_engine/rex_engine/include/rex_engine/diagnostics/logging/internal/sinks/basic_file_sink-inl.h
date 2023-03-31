

#pragma once

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/os.h>
#include <rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h>

namespace rexlog
{
  namespace sinks
  {

    template <typename Mutex>
    REXLOG_INLINE basic_file_sink<Mutex>::basic_file_sink(const filename_t& filename, bool truncate, const file_event_handlers& event_handlers)
        : file_helper_ {event_handlers}
    {
      file_helper_.open(filename, truncate);
    }

    template <typename Mutex>
    REXLOG_INLINE const filename_t& basic_file_sink<Mutex>::filename() const
    {
      return file_helper_.filename();
    }

    template <typename Mutex>
    REXLOG_INLINE void basic_file_sink<Mutex>::sink_it_(const details::log_msg& msg)
    {
      memory_buf_t formatted;
      base_sink<Mutex>::formatter_->format(msg, formatted);
      file_helper_.write(formatted);
    }

    template <typename Mutex>
    REXLOG_INLINE void basic_file_sink<Mutex>::flush_()
    {
      file_helper_.flush();
    }

  } // namespace sinks
} // namespace rexlog
