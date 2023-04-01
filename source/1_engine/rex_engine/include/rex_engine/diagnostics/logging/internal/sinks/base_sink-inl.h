

#pragma once

#include "rex_std/memory.h"
#include "rex_std/mutex.h"

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>

template <typename Mutex>
REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::base_sink()
    : formatter_ {details::make_unique<rexlog::pattern_formatter>()}
{
}

template <typename Mutex>
REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::base_sink(rsl::unique_ptr<rexlog::formatter> formatter)
    : formatter_ {rsl::move(formatter)}
{
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::log(const details::log_msg& msg)
{
  rsl::unique_lock<Mutex> lock(mutex_);
  sink_it_(msg);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::flush()
{
  rsl::unique_lock<Mutex> lock(mutex_);
  flush_();
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::set_pattern(const rex::DebugString& pattern)
{
  rsl::unique_lock<Mutex> lock(mutex_);
  set_pattern_(pattern);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter)
{
  rsl::unique_lock<Mutex> lock(mutex_);
  set_formatter_(rsl::move(sink_formatter));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::set_pattern_(const rex::DebugString& pattern)
{
  set_formatter_(details::make_unique<rexlog::pattern_formatter>(pattern));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::base_sink<Mutex>::set_formatter_(rsl::unique_ptr<rexlog::formatter> sink_formatter)
{
  formatter_ = rsl::move(sink_formatter);
}
