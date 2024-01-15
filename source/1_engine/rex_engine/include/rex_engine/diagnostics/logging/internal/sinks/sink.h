

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"

namespace rexlog
{
  namespace sinks
  {
    class AbstractSink
    {
    public:
      virtual ~AbstractSink()                                    = default;
      virtual void log(const details::LogMsg& msg)               = 0;
      virtual void flush()                                       = 0;
      virtual void set_pattern(rsl::string_view pattern)         = 0;
      virtual void set_formatter(PatternFormatter sinkFormatter) = 0;

      void set_level(level::LevelEnum logLevel);
      level::LevelEnum level() const;
      bool should_log(level::LevelEnum msgLevel) const;

    private:
      // sink log level - default is all
      level_t m_level {static_cast<s32>(level::LevelEnum::Trace)};
    };

  } // namespace sinks
} // namespace rexlog