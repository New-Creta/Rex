

#pragma once

#ifdef __ANDROID__

  #include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"
  #include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
  #include "rex_engine/diagnostics/logging/internal/details/os.h"
  #include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
  #include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

  #include <android/log.h>
  #include <chrono>
  #include <mutex>
  #include <string>
  #include <thread>
  #include <type_traits>

  #if !defined(REXLOG_ANDROID_RETRIES)
    #define REXLOG_ANDROID_RETRIES 2
  #endif

namespace rexlog
{
  namespace sinks
  {

    /*
     * Android sink
     * (logging using __android_log_write or __android_log_buf_write depending on the specified BufferID)
     */
    template <typename Mutex, int BufferID = log_id::LOG_ID_MAIN>
    class AndroidSink final : public BaseSink<Mutex>
    {
    public:
      explicit AndroidSink(rex::DebugString tag = "rexlog", bool use_raw_msg = false)
          : tag_impl(rsl::move(tag))
          , use_raw_msg_impl(use_raw_msg)
      {
      }

    protected:
      void sink_it_impl(const details::LogMsg& msg) override
      {
        const android_LogPriority priority = convert_to_android_impl(msg.level);
        memory_buf_t formatted;
        if(use_raw_msg_)
        {
          details::fmt_helper::append_string_view(msg.payload, formatted);
        }
        else
        {
          BaseSink<Mutex>::m_formatter->format(msg, formatted);
        }
        formatted.push_back('\0');
        const char* msg_output = formatted.data();

        // See system/core/liblog/logger_write.c for explanation of return value
        int ret         = android_log(priority, tag_.c_str(), msg_output);
        int retry_count = 0;
        while((ret == -11 /*EAGAIN*/) && (retry_count < REXLOG_ANDROID_RETRIES))
        {
          details::os::sleep_for_millis(5);
          ret = android_log(priority, tag_.c_str(), msg_output);
          retry_count++;
        }

        if(ret < 0)
        {
          throw_rexlog_ex("logging to Android failed", ret);
        }
      }

      void flush_impl() override {}

    private:
      // There might be liblog versions used, that do not support __android_log_buf_write. So we only compile and link against
      // __android_log_buf_write, if user explicitly provides a non-default log buffer. Otherwise, when using the default log buffer, always
      // log via __android_log_write.
      template <int ID = BufferID>
      typename rsl::enable_if<ID == static_cast<int>(log_id::LOG_ID_MAIN), int>::type android_log(int prio, const char* tag, const char* text)
      {
        return __android_log_write(prio, tag, text);
      }

      template <int ID = BufferID>
      typename rsl::enable_if<ID != static_cast<int>(log_id::LOG_ID_MAIN), int>::type android_log(int prio, const char* tag, const char* text)
      {
        return __android_log_buf_write(ID, prio, tag, text);
      }

      static android_LogPriority convert_to_android_impl(rexlog::level::level_enum level)
      {
        switch(level)
        {
          case rexlog::level::trace: return ANDROID_LOG_VERBOSE;
          case rexlog::level::debug: return ANDROID_LOG_DEBUG;
          case rexlog::level::info: return ANDROID_LOG_INFO;
          case rexlog::level::warn: return ANDROID_LOG_WARN;
          case rexlog::level::err: return ANDROID_LOG_ERROR;
          case rexlog::level::critical: return ANDROID_LOG_FATAL;
          default: return ANDROID_LOG_DEFAULT;
        }
      }

      rex::DebugString tag_;
      bool use_raw_msg_;
    };

    using android_sink_mt = AndroidSink<rsl::mutex>;
    using android_sink_st = AndroidSink<details::NullMutex>;

    template <int BufferId = log_id::LOG_ID_MAIN>
    using android_sink_buf_mt = AndroidSink<rsl::mutex, BufferId>;
    template <int BufferId = log_id::LOG_ID_MAIN>
    using android_sink_buf_st = AndroidSink<details::NullMutex, BufferId>;

  } // namespace sinks

  // Create and register android syslog Logger

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> android_logger_mt(const rex::DebugString& logger_name, const rex::DebugString& tag = "rexlog")
  {
    return Factory::template create<sinks::android_sink_mt>(logger_name, tag);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> android_logger_st(const rex::DebugString& logger_name, const rex::DebugString& tag = "rexlog")
  {
    return Factory::template create<sinks::android_sink_st>(logger_name, tag);
  }

} // namespace rexlog

#endif // __ANDROID__
