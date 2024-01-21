

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <mutex>

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      template <typename Mutex>
      class NullSink : public BaseSink<Mutex>
      {
      protected:
        void sink_it_impl(const details::LogMsg&) override {}
        void flush_it_impl() override {}
      };

      using NullSink_mt = NullSink<details::NullMutex>;
      using NullSink_st = NullSink<details::NullMutex>;

    } // namespace sinks
  }   // namespace log
} // namespace rex
