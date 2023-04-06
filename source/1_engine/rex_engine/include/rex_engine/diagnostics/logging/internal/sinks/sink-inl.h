

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

REXLOG_INLINE inline bool rexlog::sinks::sink::should_log(rexlog::level::LevelEnum msgLevel) const
{
  return msgLevel >= m_level.load(rsl::memory_order_relaxed);
}

REXLOG_INLINE inline void rexlog::sinks::sink::set_level(level::LevelEnum logLevel)
{
  m_level.store(logLevel, rsl::memory_order_relaxed);
}

REXLOG_INLINE inline rexlog::level::LevelEnum rexlog::sinks::sink::level() const
{
  return static_cast<rexlog::level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
}
