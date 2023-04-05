

#pragma once

#include "rex_std/memory.h"
#include "rex_std/mutex.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

template <typename Mutex>
REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::BaseSink()
    : m_formatter {details::make_unique<rexlog::pattern_formatter>()}
{
}

template <typename Mutex>
REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::BaseSink(rsl::unique_ptr<rexlog::formatter> formatter)
    : m_formatter {rsl::move(formatter)}
{
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::log(const details::LogMsg& msg)
{
  rsl::unique_lock<Mutex> lock(m_mutex);
  sink_it_(msg);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::flush()
{
  rsl::unique_lock<Mutex> lock(m_mutex);
  flush_();
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_pattern(const rex::DebugString& pattern)
{
  rsl::unique_lock<Mutex> lock(m_mutex);
  set_pattern_(pattern);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter)
{
  rsl::unique_lock<Mutex> lock(m_mutex);
  set_formatter_(rsl::move(sink_formatter));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_pattern_(const rex::DebugString& pattern)
{
  set_formatter_(details::make_unique<rexlog::pattern_formatter>(pattern));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_formatter_(rsl::unique_ptr<rexlog::formatter> sink_formatter)
{
  m_formatter = rsl::move(sink_formatter);
}
