

#pragma once

#include <mutex>
#include <ostream>
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class ostream_sink final : public BaseSink<Mutex>
    {
    public:
      explicit ostream_sink(rsl::ostream& os, bool force_flush = false)
          : ostream_impl(os)
          , force_flush_impl(force_flush)
      {
      }
      ostream_sink(const ostream_sink&)            = delete;
      ostream_sink& operator=(const ostream_sink&) = delete;

    protected:
      void sink_it_impl(const details::LogMsg& msg) override
      {
        memory_buf_t formatted;
        BaseSink<Mutex>::m_formatter->format(msg, formatted);
        ostream_.write(formatted.data(), static_cast<rsl::streamsize>(formatted.size()));
        if(force_flush_)
        {
          ostream_.flush();
        }
      }

      void flush_impl() override
      {
        ostream_.flush();
      }

      rsl::ostream& ostream_;
      bool force_flush_;
    };

    using ostream_sink_mt = ostream_sink<rsl::mutex>;
    using ostream_sink_st = ostream_sink<details::NullMutex>;

  } // namespace sinks
} // namespace rexlog
