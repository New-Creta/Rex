

#pragma once

#include "dist_sink.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

// Duplicate message removal sink.
// Skip the message if previous one is identical and less than "max_skip_duration" have passed
//
// Example:
//
//     #include "rex_engine/diagnostics/logging/internal/sinks/dup_filter_sink.h"
//
//     int main() {
//         auto dup_filter = rsl::make_shared<dup_filter_sink_st>(rsl::chrono::seconds(5), level::info);
//         dup_filter->add_sink(rsl::make_shared<stdout_color_sink_mt>());
//         rexlog::Logger l("Logger", dup_filter);
//         l.info("Hello");
//         l.info("Hello");
//         l.info("Hello");
//         l.info("Different Hello");
//     }
//
// Will produce:
//       [2019-06-25 17:50:56.511] [Logger] [info] Hello
//       [2019-06-25 17:50:56.512] [Logger] [info] Skipped 3 duplicate messages..
//       [2019-06-25 17:50:56.512] [Logger] [info] Different Hello

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class dup_filter_sink : public dist_sink<Mutex>
    {
    public:
      template <class Rep, class Period>
      explicit dup_filter_sink(rsl::chrono::duration<Rep, Period> max_skip_duration, level::LevelEnum notification_level = level::info);

    protected:
        void sink_it_impl(const details::LogMsg& msg) override;
        bool filter_impl(const details::LogMsg& msg);

    private:
      rsl::chrono::microseconds max_skip_duration_;
      log_clock::time_point last_msg_time_;
      rex::DebugString last_msg_payload_;
      size_t skip_counter_ = 0;
      level::LevelEnum log_level_;
    };

    template <typename Mutex>
    template <class Rep, class Period>
    dup_filter_sink<Mutex>::dup_filter_sink(rsl::chrono::duration<Rep, Period> max_skip_duration, level::LevelEnum notification_level)
        : max_skip_duration_{ max_skip_duration }
        , log_level_{ notification_level }
    {
    }

    template <typename Mutex>
    void dup_filter_sink<Mutex>::sink_it_impl(const details::LogMsg& msg)
    {
        bool filtered = filter_impl(msg);
        if (!filtered)
        {
            skip_counter_ += 1;
            return;
        }

        // log the "skipped.." message
        if (skip_counter_ > 0)
        {
            char buf[64];
            auto msg_size = ::snprintf(buf, sizeof(buf), "Skipped %u duplicate messages..", static_cast<unsigned>(skip_counter_));
            if (msg_size > 0 && static_cast<size_t>(msg_size) < sizeof(buf))
            {
                details::LogMsg skipped_msg{ msg.source, msg.logger_name, log_level_, string_view_t {buf, static_cast<size_t>(msg_size)} };
                dist_sink<Mutex>::sink_it_impl(skipped_msg);
            }
        }

        // log current message
        dist_sink<Mutex>::sink_it_impl(msg);
        last_msg_time_ = msg.time;
        skip_counter_ = 0;
        last_msg_payload_.assign(msg.payload.data(), msg.payload.data() + msg.payload.size());
    }

    template <typename Mutex>
    bool dup_filter_sink<Mutex>::filter_impl(const details::LogMsg& msg)
    {
        auto filter_duration = msg.time - last_msg_time_;
        return (filter_duration > max_skip_duration_) || (msg.payload != last_msg_payload_);
    }

    using dup_filter_sink_mt = dup_filter_sink<rsl::mutex>;
    using dup_filter_sink_st = dup_filter_sink<details::NullMutex>;

  } // namespace sinks
} // namespace rexlog
