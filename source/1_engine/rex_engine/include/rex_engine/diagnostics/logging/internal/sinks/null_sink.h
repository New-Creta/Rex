

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
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
} // namespace rexlog
