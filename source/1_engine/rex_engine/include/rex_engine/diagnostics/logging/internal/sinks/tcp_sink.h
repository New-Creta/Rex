

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#ifdef _WIN32
  #include "rex_engine/diagnostics/logging/internal/details/tcp_client-windows.h"
#else
  #include "rex_engine/diagnostics/logging/internal/details/tcp_client.h"
#endif

#include <chrono>
#include <functional>
#include <mutex>
#include <string>

#pragma once

// Simple tcp client sink
// Connects to remote address and send the formatted log.
// Will attempt to reconnect if connection drops.
// If more complicated behaviour is needed (i.e get responses), you can inherit it and override the sink_it_ method.

namespace rexlog
{
  namespace sinks
  {

    struct tcp_sink_config
    {
      rex::DebugString server_host;
      int server_port;
      bool lazy_connect = false; // if true connect on first log call instead of on construction

      tcp_sink_config(rex::DebugString host, int port)
          : server_host {rsl::move(host)}
          , server_port {port}
      {
      }
    };

    template <typename Mutex>
    class tcp_sink : public rexlog::sinks::BaseSink<Mutex>
    {
    public:
      // connect to tcp host/port or throw if failed
      // host can be hostname or ip address

      explicit tcp_sink(tcp_sink_config sink_config)
          : config_ {rsl::move(sink_config)}
      {
        if(!config_.lazy_connect)
        {
          this->client_.connect(config_.server_host, config_.server_port);
        }
      }

      ~tcp_sink() override = default;

    protected:
      void sink_it_(const rexlog::details::LogMsg& msg) override
      {
        rexlog::memory_buf_t formatted;
        rexlog::sinks::BaseSink<Mutex>::m_formatter->format(msg, formatted);
        if(!client_.is_connected())
        {
          client_.connect(config_.server_host, config_.server_port);
        }
        client_.send(formatted.data(), formatted.size());
      }

      void flush_() override {}
      tcp_sink_config config_;
      details::tcp_client client_;
    };

    using tcp_sink_mt = tcp_sink<rsl::mutex>;
    using tcp_sink_st = tcp_sink<rexlog::details::NullMutex>;

  } // namespace sinks
} // namespace rexlog
