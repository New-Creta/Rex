

#pragma once
//
// base sink templated over a mutex (either dummy or real)
// concrete implementation should override the sink_it_impl() and flush_impl()  methods.
// locking is taken care of in this class - no locking needed by the
// implementers..
//

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class REXLOG_API BaseSink : public sink
    {
    public:
      BaseSink();
      explicit BaseSink(rsl::unique_ptr<rexlog::formatter> formatter);
      ~BaseSink() override = default;

      BaseSink(const BaseSink&) = delete;
      BaseSink(BaseSink&&)      = delete;

      BaseSink& operator=(const BaseSink&) = delete;
      BaseSink& operator=(BaseSink&&)      = delete;

      void log(const details::LogMsg& msg) final;
      void flush() final;
      void set_pattern(const rex::DebugString& pattern) final;
      void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) final;

    protected:
      // sink formatter
      rsl::unique_ptr<rexlog::formatter> m_formatter;
      Mutex m_mutex;

      virtual void sink_it_impl(const details::LogMsg& msg) = 0;
      virtual void flush_impl()                              = 0;
      virtual void set_pattern_impl(const rex::DebugString& pattern);
      virtual void set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sink_formatter);
    };
  } // namespace sinks
} // namespace rexlog