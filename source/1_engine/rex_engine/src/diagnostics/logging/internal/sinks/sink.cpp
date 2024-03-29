#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_std/bonus/atomic/atomic.h"

// IWYU pragma: no_include <built-in>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      bool AbstractSink::should_log(rex::log::level::LevelEnum msgLevel) const
      {
        return static_cast<s32>(msgLevel) >= m_level.load(rsl::memory_order_relaxed);
      }

      void AbstractSink::set_level(level::LevelEnum logLevel)
      {
        m_level.store(static_cast<s32>(logLevel), rsl::memory_order_relaxed);
      }

      rex::log::level::LevelEnum AbstractSink::level() const
      {
        return static_cast<rex::log::level::LevelEnum>(m_level.load(rsl::memory_order_relaxed));
      }
    } // namespace sinks
  }   // namespace log
} // namespace rex

// NOLINTEND(misc-definitions-in-headers)
