
#pragma once

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <array>
#ifndef SD_JOURNAL_SUPPRESS_LOCATION
  #define SD_JOURNAL_SUPPRESS_LOCATION
#endif
#include <systemd/sd-journal.h>

namespace rexlog
{
  namespace sinks
  {

    /**
     * Sink that write to systemd journal using the `sd_journal_send()` library call.
     */
    template <typename Mutex>
    class systemd_sink : public BaseSink<Mutex>
    {
    public:
      systemd_sink(rex::DebugString ident = "", bool enable_formatting = false)
          : ident_ {rsl::move(ident)}
          , enable_formatting_ {enable_formatting}
          , syslog_levels_ {{/* rexlog::level::trace      */ LOG_DEBUG,
                             /* rexlog::level::debug      */ LOG_DEBUG,
                             /* rexlog::level::info       */ LOG_INFO,
                             /* rexlog::level::warn       */ LOG_WARNING,
                             /* rexlog::level::err        */ LOG_ERR,
                             /* rexlog::level::critical   */ LOG_CRIT,
                             /* rexlog::level::off        */ LOG_INFO}}
      {
      }

      ~systemd_sink() override {}

      systemd_sink(const systemd_sink&)            = delete;
      systemd_sink& operator=(const systemd_sink&) = delete;

    protected:
      const rex::DebugString ident_;
      bool enable_formatting_ = false;
      using levels_array      = rsl::array<int, 7>;
      levels_array syslog_levels_;

      void sink_it_impl(const details::LogMsg& msg) override
      {
        int err;
        string_view_t payload;
        memory_buf_t formatted;
        if(enable_formatting_)
        {
          BaseSink<Mutex>::m_formatter->format(msg, formatted);
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

        const string_view_t syslog_identifier = ident_.empty() ? msg.logger_name : ident_;

        // Do not send source location if not available
        if(msg.source.empty())
        {
          // Note: function call inside '()' to avoid macro expansion
          err = (sd_journal_send)("MESSAGE=%.*s", static_cast<int>(length), payload.data(), "PRIORITY=%d", syslog_level(msg.level),
#ifndef REXLOG_NO_THREAD_ID
                                  "TID=%zu", details::os::thread_id(),
#endif
                                  "SYSLOG_IDENTIFIER=%.*s", static_cast<int>(syslog_identifier.size()), syslog_identifier.data(), nullptr);
        }
        else
        {
          err = (sd_journal_send)("MESSAGE=%.*s", static_cast<int>(length), payload.data(), "PRIORITY=%d", syslog_level(msg.level),
#ifndef REXLOG_NO_THREAD_ID
                                  "TID=%zu", details::os::thread_id(),
#endif
                                  "SYSLOG_IDENTIFIER=%.*s", static_cast<int>(syslog_identifier.size()), syslog_identifier.data(), "CODE_FILE=%s", msg.source.filename, "CODE_LINE=%d", msg.source.line, "CODE_FUNC=%s", msg.source.funcname, nullptr);
        }

        if(err)
        {
          throw_rexlog_ex("Failed writing to systemd", errno);
        }
      }

      int syslog_level(level::level_enum l)
      {
        return syslog_levels_.at(static_cast<levels_array::size_type>(l));
      }

      void flush_impl() override {}
    };

    using systemd_sink_mt = systemd_sink<rsl::mutex>;
    using systemd_sink_st = systemd_sink<details::NullMutex>;
  } // namespace sinks

  // Create and register a syslog Logger
  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> systemd_logger_mt(const rex::DebugString& logger_name, const rex::DebugString& ident = "", bool enable_formatting = false)
  {
    return Factory::template create<sinks::systemd_sink_mt>(logger_name, ident, enable_formatting);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> systemd_logger_st(const rex::DebugString& logger_name, const rex::DebugString& ident = "", bool enable_formatting = false)
  {
    return Factory::template create<sinks::systemd_sink_st>(logger_name, ident, enable_formatting);
  }
} // namespace rexlog
