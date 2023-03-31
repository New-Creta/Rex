

#pragma once

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#ifdef _WIN32
#    include <rex_engine/diagnostics/logging/internal/details/udp_client-windows.h>
#else
#    include <rex_engine/diagnostics/logging/internal/details/udp_client.h>
#endif

#include <mutex>
#include <string>
#include <chrono>
#include <functional>

// Simple udp client sink
// Sends formatted log via udp

namespace rexlog {
namespace sinks {

struct udp_sink_config
{
    rsl::string server_host;
    uint16_t server_port;

    udp_sink_config(rsl::string host, uint16_t port)
        : server_host{rsl::move(host)}
        , server_port{port}
    {}
};

template<typename Mutex>
class udp_sink : public rexlog::sinks::base_sink<Mutex>
{
public:
    // host can be hostname or ip address
    explicit udp_sink(udp_sink_config sink_config)
        : client_{sink_config.server_host, sink_config.server_port}
    {}

    ~udp_sink() override = default;

protected:
    void sink_it_(const rexlog::details::log_msg &msg) override
    {
        rexlog::memory_buf_t formatted;
        rexlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        client_.send(formatted.data(), formatted.size());
    }

    void flush_() override {}
    details::udp_client client_;
};

using udp_sink_mt = udp_sink<rsl::mutex>;
using udp_sink_st = udp_sink<rexlog::details::null_mutex>;

} // namespace sinks

//
// factory functions
//
template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> udp_logger_mt(const rsl::string &logger_name, sinks::udp_sink_config skin_config)
{
    return Factory::template create<sinks::udp_sink_mt>(logger_name, skin_config);
}

} // namespace rexlog
