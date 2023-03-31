

#pragma once

#include <rex_engine/diagnostics/logging/internal/details/backtracer.h>

#include "rex_std/mutex.h"

namespace rexlog {
namespace details {
REXLOG_INLINE backtracer::backtracer(const backtracer &other)
{
    rsl::unique_lock<rsl::mutex> lock(other.mutex_);
    enabled_ = other.enabled();
    messages_ = other.messages_;
}

REXLOG_INLINE backtracer::backtracer(backtracer &&other) REXLOG_NOEXCEPT
{
    rsl::unique_lock<rsl::mutex> lock(other.mutex_);
    enabled_ = other.enabled();
    messages_ = rsl::move(other.messages_);
}

REXLOG_INLINE backtracer &backtracer::operator=(backtracer other)
{
    rsl::unique_lock<rsl::mutex> lock(mutex_);
    enabled_ = other.enabled();
    messages_ = rsl::move(other.messages_);
    return *this;
}

REXLOG_INLINE void backtracer::enable(size_t size)
{
    rsl::unique_lock<rsl::mutex> lock{mutex_};
    enabled_.store(true, rsl::memory_order_relaxed);
    messages_ = circular_q<log_msg_buffer>{size};
}

REXLOG_INLINE void backtracer::disable()
{
    rsl::unique_lock<rsl::mutex> lock{mutex_};
    enabled_.store(false, rsl::memory_order_relaxed);
}

REXLOG_INLINE bool backtracer::enabled() const
{
    return enabled_.load(rsl::memory_order_relaxed);
}

REXLOG_INLINE void backtracer::push_back(const log_msg &msg)
{
    rsl::unique_lock<rsl::mutex> lock{mutex_};
    messages_.push_back(log_msg_buffer{msg});
}

REXLOG_INLINE bool backtracer::empty() const
{
    rsl::unique_lock<rsl::mutex> lock{mutex_};
    return messages_.empty();
}

// pop all items in the q and apply the given fun on each of them.
REXLOG_INLINE void backtracer::foreach_pop(rsl::function<void(const details::log_msg &)> fun)
{
    rsl::unique_lock<rsl::mutex> lock{mutex_};
    while (!messages_.empty())
    {
        auto &front_msg = messages_.front();
        fun(front_msg);
        messages_.pop_front();
    }
}
} // namespace details
} // namespace rexlog
