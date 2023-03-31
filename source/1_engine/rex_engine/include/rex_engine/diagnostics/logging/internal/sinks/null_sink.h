

#pragma once

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/details/synchronous_factory.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>

namespace rexlog
{
  namespace sinks
  {

    template <typename Mutex>
    class null_sink : public base_sink<Mutex>
    {
    protected:
      void sink_it_(const details::log_msg&) override {}
      void flush_() override {}
    };

    using null_sink_mt = null_sink<details::null_mutex>;
    using null_sink_st = null_sink<details::null_mutex>;

  } // namespace sinks

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> null_logger_mt(const rsl::string& logger_name)
  {
    auto null_logger = Factory::template create<sinks::null_sink_mt>(logger_name);
    null_logger->set_level(level::off);
    return null_logger;
  }

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> null_logger_st(const rsl::string& logger_name)
  {
    auto null_logger = Factory::template create<sinks::null_sink_st>(logger_name);
    null_logger->set_level(level::off);
    return null_logger;
  }

} // namespace rexlog
