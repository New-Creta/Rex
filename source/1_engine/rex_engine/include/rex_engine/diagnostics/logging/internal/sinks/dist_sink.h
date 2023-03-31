

#pragma once

#include "base_sink.h"
#include <rex_engine/diagnostics/logging/internal/details/log_msg.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter.h>

#include <algorithm>
#include "rex_std/memory.h"
#include <mutex>
#include "rex_std/vector.h"

// Distribution sink (mux). Stores a vector of sinks which get called when log
// is called

namespace rexlog {
namespace sinks {

template<typename Mutex>
class dist_sink : public base_sink<Mutex>
{
public:
    dist_sink() = default;
    explicit dist_sink(rsl::vector<rsl::shared_ptr<sink>> sinks)
        : sinks_(sinks)
    {}

    dist_sink(const dist_sink &) = delete;
    dist_sink &operator=(const dist_sink &) = delete;

    void add_sink(rsl::shared_ptr<sink> sub_sink)
    {
        rsl::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        sinks_.push_back(sub_sink);
    }

    void remove_sink(rsl::shared_ptr<sink> sub_sink)
    {
        rsl::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        sinks_.erase(rsl::remove(sinks_.begin(), sinks_.end(), sub_sink), sinks_.end());
    }

    void set_sinks(rsl::vector<rsl::shared_ptr<sink>> sinks)
    {
        rsl::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        sinks_ = rsl::move(sinks);
    }

    rsl::vector<rsl::shared_ptr<sink>> &sinks()
    {
        return sinks_;
    }

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        for (auto &sub_sink : sinks_)
        {
            if (sub_sink->should_log(msg.level))
            {
                sub_sink->log(msg);
            }
        }
    }

    void flush_() override
    {
        for (auto &sub_sink : sinks_)
        {
            sub_sink->flush();
        }
    }

    void set_pattern_(const rsl::string &pattern) override
    {
        set_formatter_(details::make_unique<rexlog::pattern_formatter>(pattern));
    }

    void set_formatter_(rsl::unique_ptr<rexlog::formatter> sink_formatter) override
    {
        base_sink<Mutex>::formatter_ = rsl::move(sink_formatter);
        for (auto &sub_sink : sinks_)
        {
            sub_sink->set_formatter(base_sink<Mutex>::formatter_->clone());
        }
    }
    rsl::vector<rsl::shared_ptr<sink>> sinks_;
};

using dist_sink_mt = dist_sink<rsl::mutex>;
using dist_sink_st = dist_sink<details::null_mutex>;

} // namespace sinks
} // namespace rexlog
