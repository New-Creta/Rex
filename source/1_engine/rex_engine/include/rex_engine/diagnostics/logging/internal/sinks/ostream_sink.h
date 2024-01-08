

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_std/bonus/utility/yes_no.h"

#include <mutex>
#include <ostream>

namespace rexlog
{
  namespace sinks
  {
    DEFINE_YES_NO_ENUM(ForceFlush);

    template <typename Mutex>
    class OStreamSink final : public BaseSink<Mutex>
    {
    public:
      explicit OStreamSink(rsl::ostream& os, ForceFlush force_flush = ForceFlush::no);

      OStreamSink(const OStreamSink&)            = delete;
      OStreamSink& operator=(const OStreamSink&) = delete;

    protected:
      void sink_it_impl(const details::LogMsg& msg) override;
      void flush_it_impl() override;

    private:
      rsl::ostream& ostream_;
      ForceFlush force_flush_;
    };

    template <typename Mutex>
    OStreamSink<Mutex>::OStreamSink(rsl::ostream& os, ForceFlush force_flush)
        : ostream_impl(os)
        , force_flush_impl(force_flush)
    {
    }

    template <typename Mutex>
    void OStreamSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
    {
      memory_buf_t formatted;
      BaseSink<Mutex>::m_formatter->format(msg, formatted);
      ostream_.write(formatted.data(), static_cast<rsl::streamsize>(formatted.size()));
      if(force_flush_)
      {
        ostream_.flush();
      }
    }

    template <typename Mutex>
    void OStreamSink<Mutex>::flush_it_impl()
    {
      ostream_.flush();
    }

    using OStreamSink_mt = OStreamSink<rsl::mutex>;
    using OStreamSink_st = OStreamSink<details::NullMutex>;

  } // namespace sinks
} // namespace rexlog
