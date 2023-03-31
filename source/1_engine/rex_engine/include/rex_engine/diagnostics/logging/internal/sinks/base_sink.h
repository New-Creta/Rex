

#pragma once
//
// base sink templated over a mutex (either dummy or real)
// concrete implementation should override the sink_it_() and flush_()  methods.
// locking is taken care of in this class - no locking needed by the
// implementers..
//

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/log_msg.h>
#include <rex_engine/diagnostics/logging/internal/sinks/sink.h>

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class REXLOG_API base_sink : public sink
    {
    public:
      base_sink();
      explicit base_sink(rsl::unique_ptr<rexlog::formatter> formatter);
      ~base_sink() override = default;

      base_sink(const base_sink&) = delete;
      base_sink(base_sink&&)      = delete;

      base_sink& operator=(const base_sink&) = delete;
      base_sink& operator=(base_sink&&)      = delete;

      void log(const details::log_msg& msg) final;
      void flush() final;
      void set_pattern(const rsl::string& pattern) final;
      void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) final;

    protected:
      // sink formatter
      rsl::unique_ptr<rexlog::formatter> formatter_;
      Mutex mutex_;

      virtual void sink_it_(const details::log_msg& msg) = 0;
      virtual void flush_()                              = 0;
      virtual void set_pattern_(const rsl::string& pattern);
      virtual void set_formatter_(rsl::unique_ptr<rexlog::formatter> sink_formatter);
    };
  } // namespace sinks
} // namespace rexlog