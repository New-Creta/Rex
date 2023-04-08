

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"

template <typename Mutex>
REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::BaseSink()
    : m_formatter {details::make_unique<rexlog::PatternFormatter>()}
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
  const rsl::unique_lock<Mutex> lock(m_mutex);
  sink_it_impl(msg);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::flush()
{
  const rsl::unique_lock<Mutex> lock(m_mutex);
  flush_impl();
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_pattern(const rex::DebugString& pattern)
{
  const rsl::unique_lock<Mutex> lock(m_mutex);
  set_pattern_impl(pattern);
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
{
  const rsl::unique_lock<Mutex> lock(m_mutex);
  set_formatter_impl(rsl::move(sinkFormatter));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_pattern_impl(const rex::DebugString& pattern)
{
  set_formatter_impl(details::make_unique<rexlog::PatternFormatter>(pattern));
}

template <typename Mutex>
void REXLOG_INLINE rexlog::sinks::BaseSink<Mutex>::set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
{
  m_formatter = rsl::move(sinkFormatter);
}

template <typename Mutex>
rexlog::formatter* rexlog::sinks::BaseSink<Mutex>::formatter()
{
  return m_formatter.get();
}

template <typename Mutex>
Mutex& rexlog::sinks::BaseSink<Mutex>::mutex()
{
  return m_mutex;
}