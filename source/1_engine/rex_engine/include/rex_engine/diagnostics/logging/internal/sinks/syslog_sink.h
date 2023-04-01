

#pragma once

#include <array>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/details/synchronous_factory.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>
#include <string>
#include <syslog.h>

namespace rexlog
{
  namespace sinks
  {
    /**
     * Sink that write to syslog using the `syscall()` library call.
     */
    template <typename Mutex>
    class syslog_sink : public base_sink<Mutex>
    {
    public:
      syslog_sink(rex::DebugString ident, int syslog_option, int syslog_facility, bool enable_formatting)
          : enable_formatting_ {enable_formatting}
          , syslog_levels_ {{/* rexlog::level::trace      */ LOG_DEBUG,
                             /* rexlog::level::debug      */ LOG_DEBUG,
                             /* rexlog::level::info       */ LOG_INFO,
                             /* rexlog::level::warn       */ LOG_WARNING,
                             /* rexlog::level::err        */ LOG_ERR,
                             /* rexlog::level::critical   */ LOG_CRIT,
                             /* rexlog::level::off        */ LOG_INFO}}
          , ident_ {rsl::move(ident)}
      {
        // set ident to be program name if empty
        ::openlog(ident_.empty() ? nullptr : ident_.c_str(), syslog_option, syslog_facility);
      }

      ~syslog_sink() override
      {
        ::closelog();
      }

      syslog_sink(const syslog_sink&)            = delete;
      syslog_sink& operator=(const syslog_sink&) = delete;

    protected:
      void sink_it_(const details::log_msg& msg) override
      {
        string_view_t payload;
        memory_buf_t formatted;
        if(enable_formatting_)
        {
          base_sink<Mutex>::formatter_->format(msg, formatted);
          payload = string_view_t(formatted.data(), formatted.size());
        }
        else
        {
          payload = msg.payload;
        }

        size_t length = payload.size();
        // limit to max int
        if(length > static_cast<size_t>(rsl::numeric_limits<int>::max()))
        {
          length = static_cast<size_t>(rsl::numeric_limits<int>::max());
        }

        ::syslog(syslog_prio_from_level(msg), "%.*s", static_cast<int>(length), payload.data());
      }

      void flush_() override {}
      bool enable_formatting_ = false;

    private:
      using levels_array = rsl::array<int, 7>;
      levels_array syslog_levels_;
      // must store the ident because the man says openlog might use the pointer as
      // is and not a string copy
      const rex::DebugString ident_;

      //
      // Simply maps rexlog's log level to syslog priority level.
      //
      int syslog_prio_from_level(const details::log_msg& msg) const
      {
        return syslog_levels_.at(static_cast<levels_array::size_type>(msg.level));
      }
    };

    using syslog_sink_mt = syslog_sink<rsl::mutex>;
    using syslog_sink_st = syslog_sink<details::null_mutex>;
  } // namespace sinks

  // Create and register a syslog logger
  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> syslog_logger_mt(const rex::DebugString& logger_name, const rex::DebugString& syslog_ident = "", int syslog_option = 0, int syslog_facility = LOG_USER, bool enable_formatting = false)
  {
    return Factory::template create<sinks::syslog_sink_mt>(logger_name, syslog_ident, syslog_option, syslog_facility, enable_formatting);
  }

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> syslog_logger_st(const rex::DebugString& logger_name, const rex::DebugString& syslog_ident = "", int syslog_option = 0, int syslog_facility = LOG_USER, bool enable_formatting = false)
  {
    return Factory::template create<sinks::syslog_sink_st>(logger_name, syslog_ident, syslog_option, syslog_facility, enable_formatting);
  }
} // namespace rexlog
