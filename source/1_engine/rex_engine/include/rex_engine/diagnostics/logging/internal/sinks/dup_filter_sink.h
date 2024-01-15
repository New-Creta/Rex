

#pragma once

#include "DistSink.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

// Duplicate message removal sink.
// Skip the message if previous one is identical and less than "max_skip_duration" have passed
//
// Example:
//
//     #include "rex_engine/diagnostics/logging/internal/sinks/DupFilterSink.h"
//
//     int main() {
//         auto dup_filter = rsl::make_shared<DupFilterSink_st>(rsl::chrono::seconds(5), level::info);
//         dup_filter->add_sink(rsl::make_shared<StdoutColorSinkMt>());
//         rex::log::Logger l("Logger", dup_filter);
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

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      template <typename Mutex>
      class DupFilterSink : public DistSink<Mutex>
      {
      public:
        template <class Rep, class Period>
        explicit DupFilterSink(rsl::chrono::duration<Rep, Period> max_skip_duration, level::LevelEnum notification_level = level::info);

      protected:
        void sink_it_impl(const details::LogMsg& msg) override;
        bool filter_impl(const details::LogMsg& msg);

      private:
        rsl::chrono::microseconds max_skip_duration_;
        log_clock::time_point last_msg_time_;
        rex::DebugString last_msg_payload_;
        s32 skip_counter_ = 0;
        level::LevelEnum log_level_;
      };

      template <typename Mutex>
      template <class Rep, class Period>
      DupFilterSink<Mutex>::DupFilterSink(rsl::chrono::duration<Rep, Period> max_skip_duration, level::LevelEnum notification_level)
          : max_skip_duration_ {max_skip_duration}
          , log_level_ {notification_level}
      {
      }

      template <typename Mutex>
      void DupFilterSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
      {
        bool filtered = filter_impl(msg);
        if(!filtered)
        {
          skip_counter_ += 1;
          return;
        }

        // log the "skipped.." message
        if(skip_counter_ > 0)
        {
          rsl::small_stack_string buf;
          auto msg_size = ::snprintf(buf.data(), buf.size(), "Skipped %u duplicate messages..", static_cast<unsigned>(skip_counter_));
          if(msg_size > 0 && static_cast<s32>(msg_size) < buf.size())
          {
            details::LogMsg skipped_msg {msg.source, msg.logger_name, log_level_, rsl::string_view {buf.data(), static_cast<s32>(msg_size)}};
            DistSink<Mutex>::sink_it_impl(skipped_msg);
          }
        }

        // log current message
        DistSink<Mutex>::sink_it_impl(msg);
        last_msg_time_ = msg.time;
        skip_counter_  = 0;
        last_msg_payload_.assign(msg.payload.data(), msg.payload.data() + msg.payload.size());
      }

      template <typename Mutex>
      bool DupFilterSink<Mutex>::filter_impl(const details::LogMsg& msg)
      {
        auto filter_duration = msg.time - last_msg_time_;
        return (filter_duration > max_skip_duration_) || (msg.payload != last_msg_payload_);
      }

      using DupFilterSink_mt = DupFilterSink<rsl::mutex>;
      using DupFilterSink_st = DupFilterSink<details::NullMutex>;

    } // namespace sinks
  }   // namespace log
} // namespace rex
