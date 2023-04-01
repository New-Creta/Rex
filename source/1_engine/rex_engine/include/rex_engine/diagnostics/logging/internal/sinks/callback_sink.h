

#pragma once

#include "rex_std/functional.h"
#include "rex_std/mutex.h"
#include "rex_std/string.h"

#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/details/synchronous_factory.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>

namespace rexlog
{

  // callbacks type
  typedef rsl::function<void(const details::log_msg& msg)> custom_log_callback;

  namespace sinks
  {
    /*
     * Trivial callback sink, gets a callback function and calls it on each log
     */
    template <typename Mutex>
    class callback_sink final : public base_sink<Mutex>
    {
    public:
      explicit callback_sink(const custom_log_callback& callback)
          : callback_ {callback}
      {
      }

    protected:
      void sink_it_(const details::log_msg& msg) override
      {
        callback_(msg);
      }
      void flush_() override {};

    private:
      custom_log_callback callback_;
    };

    using callback_sink_mt = callback_sink<rsl::mutex>;
    using callback_sink_st = callback_sink<details::null_mutex>;

  } // namespace sinks

  //
  // factory functions
  //
  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> callback_logger_mt(const rex::DebugString& logger_name, const custom_log_callback& callback)
  {
    return Factory::template create<sinks::callback_sink_mt>(logger_name, callback);
  }

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> callback_logger_st(const rex::DebugString& logger_name, const custom_log_callback& callback)
  {
    return Factory::template create<sinks::callback_sink_st>(logger_name, callback);
  }

} // namespace rexlog
