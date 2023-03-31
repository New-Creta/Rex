

#pragma once

#include <rex_engine/diagnostics/logging/internal/details/log_msg.h>
#include <rex_engine/diagnostics/logging/internal/formatter.h>

namespace rexlog {

namespace sinks {
class REXLOG_API sink
{
public:
    virtual ~sink() = default;
    virtual void log(const details::log_msg &msg) = 0;
    virtual void flush() = 0;
    virtual void set_pattern(const rsl::string &pattern) = 0;
    virtual void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) = 0;

    void set_level(level::level_enum log_level);
    level::level_enum level() const;
    bool should_log(level::level_enum msg_level) const;

protected:
    // sink log level - default is all
    level_t level_{level::trace};
};

} // namespace sinks
} // namespace rexlog