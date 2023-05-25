#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/rexlog.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"

#include "rex_std/mutex.h"

namespace rexlog
{
    //-------------------------------------------------------------------------
    void initialize_logger(rsl::shared_ptr<Logger> logger)
    {
        details::Registry::instance().initialize_logger(rsl::move(logger));
    }

    //-------------------------------------------------------------------------
    void register_logger(rsl::shared_ptr<Logger> logger)
    {
        details::Registry::instance().register_logger(rsl::move(logger));
    }

    //-------------------------------------------------------------------------
    rsl::shared_ptr<Logger> get(rsl::string_view name)
    {
        return details::Registry::instance().get(name);
    }

    //-------------------------------------------------------------------------
    void set_formatter(PatternFormatter&& formatter)
    {
        details::Registry::instance().set_formatter(rsl::move(formatter));
    }

    //-------------------------------------------------------------------------
    void set_pattern(const rsl::small_stack_string& pattern, PatternTimeType timeType)
    {
        set_formatter(PatternFormatter(rsl::move(pattern), timeType));
    }

    //-------------------------------------------------------------------------
    void set_global_level(level::LevelEnum logLevel)
    {
        details::Registry::instance().set_level(logLevel);
    }

    //-------------------------------------------------------------------------
    rexlog::level::LevelEnum global_level()
    {
        return details::Registry::instance().get_global_level();
    }

    //-------------------------------------------------------------------------
    void flush_on(level::LevelEnum logLevel)
    {
        details::Registry::instance().flush_on(logLevel);
    }

    //-------------------------------------------------------------------------
    void flush_all()
    {
        details::Registry::instance().flush_all();
    }

    //-------------------------------------------------------------------------
    void shutdown()
    {
        details::Registry::instance().shutdown();
    }
} // namespace rexlog
