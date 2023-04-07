

#pragma once

#include "rex_engine/types.h"

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/formatter.h"

namespace rexlog
{

  namespace sinks
  {
    class REXLOG_API sink
    {
    public:
      virtual ~sink()                                                               = default;
      virtual void log(const details::LogMsg& msg)                                 = 0;
      virtual void flush()                                                          = 0;
      virtual void set_pattern(const rex::DebugString& pattern)                     = 0;
      virtual void set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter) = 0;

      void set_level(level::LevelEnum logLevel);
      level::LevelEnum level() const;
      bool should_log(level::LevelEnum msgLevel) const;

    private:
      // sink log level - default is all
      level_t m_level {level::Trace};
    };

  } // namespace sinks
} // namespace rexlog