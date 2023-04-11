

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_std/iostream.h"
#include <Windows.h>
// NOLINTBEGIN(misc-definitions-in-headers)

REXLOG_INLINE bool rexlog::sinks::sink::should_log(rexlog::level::LevelEnum msgLevel) const
{
  rsl::cout << "msg level: " << (int)msgLevel << "\n";
  rsl::cout << "level: " << (int)m_level.load(rsl::memory_order_relaxed) << "\n";
  rsl::cout << "threadid: " << GetCurrentThreadId() << "\n";
  return msgLevel >= m_level.load(rsl::memory_order_relaxed);
}

REXLOG_INLINE void rexlog::sinks::sink::set_level(level::LevelEnum logLevel)
{
  rsl::cout << "threadid: " << GetCurrentThreadId() << "\n";
  m_level.store(logLevel, rsl::memory_order_relaxed);
}

REXLOG_INLINE rexlog::level::LevelEnum rexlog::sinks::sink::level() const
{
  return static_cast<rexlog::level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
}

// NOLINTEND(misc-definitions-in-headers)
