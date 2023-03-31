

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/circular_q.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_std/vector.h"

#include <mutex>
#include <string>

namespace rexlog
{
  namespace sinks
  {
    /*
     * Ring buffer sink
     */
    template <typename Mutex>
    class ringbuffer_sink final : public base_sink<Mutex>
    {
    public:
      explicit ringbuffer_sink(size_t n_items)
          : q_ {n_items}
      {
      }

      rsl::vector<details::log_msg_buffer> last_raw(size_t lim = 0)
      {
        rsl::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        auto items_available = q_.size();
        auto n_items         = lim > 0 ? (rsl::min)(lim, items_available) : items_available;
        rsl::vector<details::log_msg_buffer> ret;
        ret.reserve(n_items);
        for(size_t i = (items_available - n_items); i < items_available; i++)
        {
          ret.push_back(q_.at(i));
        }
        return ret;
      }

      rsl::vector<rsl::string> last_formatted(size_t lim = 0)
      {
        rsl::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        auto items_available = q_.size();
        auto n_items         = lim > 0 ? (rsl::min)(lim, items_available) : items_available;
        rsl::vector<rsl::string> ret;
        ret.reserve(n_items);
        for(size_t i = (items_available - n_items); i < items_available; i++)
        {
          memory_buf_t formatted;
          base_sink<Mutex>::formatter_->format(q_.at(i), formatted);
          ret.push_back(rsl::move(REXLOG_BUF_TO_STRING(formatted)));
        }
        return ret;
      }

    protected:
      void sink_it_(const details::log_msg& msg) override
      {
        q_.push_back(details::log_msg_buffer {msg});
      }
      void flush_() override {}

    private:
      details::circular_q<details::log_msg_buffer> q_;
    };

    using ringbuffer_sink_mt = ringbuffer_sink<rsl::mutex>;
    using ringbuffer_sink_st = ringbuffer_sink<details::null_mutex>;

  } // namespace sinks

} // namespace rexlog
