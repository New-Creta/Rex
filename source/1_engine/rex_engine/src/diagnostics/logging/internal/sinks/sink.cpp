

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    namespace sinks
    {
        bool AbstractSink::should_log(rexlog::level::LevelEnum msgLevel) const
        {
            return msgLevel >= m_level.load(rsl::memory_order_relaxed);
        }

        void AbstractSink::set_level(level::LevelEnum logLevel)
        {
            m_level.store(logLevel, rsl::memory_order_relaxed);
        }

        rexlog::level::LevelEnum AbstractSink::level() const
        {
            return static_cast<rexlog::level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
        }
    }
}


// NOLINTEND(misc-definitions-in-headers)
