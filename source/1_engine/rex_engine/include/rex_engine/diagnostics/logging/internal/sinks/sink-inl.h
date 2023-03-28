

#pragma once

#include <rexlog/sinks/sink.h>

#include <rexlog/common.h>

REXLOG_INLINE bool rexlog::sinks::sink::should_log(rexlog::level::level_enum msg_level) const
{
    return msg_level >= level_.load(rsl::memory_order_relaxed);
}

REXLOG_INLINE void rexlog::sinks::sink::set_level(level::level_enum log_level)
{
    level_.store(log_level, rsl::memory_order_relaxed);
}

REXLOG_INLINE rexlog::level::level_enum rexlog::sinks::sink::level() const
{
    return static_cast<rexlog::level::level_enum>(level_.load(rsl::memory_order_relaxed));
}
