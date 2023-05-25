

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <mutex>

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class null_sink : public BaseSink<Mutex>
    {
    protected:
      void sink_it_impl(const details::LogMsg&) override {}
      void flush_it_impl() override {}
    };

    using null_sink_mt = null_sink<details::NullMutex>;
    using null_sink_st = null_sink<details::NullMutex>;

  } // namespace sinks

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> null_logger_mt(const rex::DebugString& logger_name)
  {
    auto null_logger = Factory::template create<sinks::null_sink_mt>(logger_name);
    null_logger->set_level(level::off);
    return null_logger;
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> null_logger_st(const rex::DebugString& logger_name)
  {
    auto null_logger = Factory::template create<sinks::null_sink_st>(logger_name);
    null_logger->set_level(level::off);
    return null_logger;
  }

} // namespace rexlog
