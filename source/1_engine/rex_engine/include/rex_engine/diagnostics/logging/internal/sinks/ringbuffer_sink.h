

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
    class ringbuffer_sink final : public BaseSink<Mutex>
    {
    public:
      explicit ringbuffer_sink(size_t n_items)
          : m_q {n_items}
      {
      }

      rex::DebugVector<details::LogMsgBuffer> last_raw(size_t lim = 0)
      {
        rsl::lock_guard<Mutex> lock(BaseSink<Mutex>::m_mutex);
        auto items_available = m_q.size();
        auto n_items         = lim > 0 ? (rsl::min)(lim, items_available) : items_available;
        rex::DebugVector<details::LogMsgBuffer> ret;
        ret.reserve(n_items);
        for(size_t i = (items_available - n_items); i < items_available; i++)
        {
          ret.push_back(m_q.at(i));
        }
        return ret;
      }

      rex::DebugVector<rex::DebugString> last_formatted(size_t lim = 0)
      {
        rsl::lock_guard<Mutex> lock(BaseSink<Mutex>::m_mutex);
        auto items_available = m_q.size();
        auto n_items         = lim > 0 ? (rsl::min)(lim, items_available) : items_available;
        rex::DebugVector<rex::DebugString> ret;
        ret.reserve(n_items);
        for(size_t i = (items_available - n_items); i < items_available; i++)
        {
          memory_buf_t formatted;
          BaseSink<Mutex>::m_formatter->format(m_q.at(i), formatted);
          ret.push_back(rsl::move(REXLOG_BUF_TO_STRING(formatted)));
        }
        return ret;
      }

    protected:
      void sink_it_impl(const details::LogMsg& msg) override
      {
        m_q.push_back(details::LogMsgBuffer {msg});
      }
      void flush_impl() override {}

    private:
      details::CircularQ<details::LogMsgBuffer> m_q;
    };

    using ringbuffer_sink_mt = ringbuffer_sink<rsl::mutex>;
    using ringbuffer_sink_st = ringbuffer_sink<details::NullMutex>;

  } // namespace sinks

} // namespace rexlog
