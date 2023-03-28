

#pragma once

#include <rexlog/details/null_mutex.h>
#include <rexlog/sinks/base_sink.h>

#include <mutex>
#include <ostream>

namespace rexlog {
namespace sinks {
template<typename Mutex>
class ostream_sink final : public base_sink<Mutex>
{
public:
    explicit ostream_sink(rsl::ostream &os, bool force_flush = false)
        : ostream_(os)
        , force_flush_(force_flush)
    {}
    ostream_sink(const ostream_sink &) = delete;
    ostream_sink &operator=(const ostream_sink &) = delete;

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        ostream_.write(formatted.data(), static_cast<rsl::streamsize>(formatted.size()));
        if (force_flush_)
        {
            ostream_.flush();
        }
    }

    void flush_() override
    {
        ostream_.flush();
    }

    rsl::ostream &ostream_;
    bool force_flush_;
};

using ostream_sink_mt = ostream_sink<rsl::mutex>;
using ostream_sink_st = ostream_sink<details::null_mutex>;

} // namespace sinks
} // namespace rexlog
