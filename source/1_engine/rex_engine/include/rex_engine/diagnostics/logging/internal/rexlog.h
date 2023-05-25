

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/rexlog_logger.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/string/stack_string.h"

namespace rexlog
{
    using default_factory = SynchronousFactory;

    // Create and register a Logger with a templated sink type
    // The Logger's level, formatter and flush level will be set according the
    // global settings.
    //
    // Example:
    //   rexlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return default_factory::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }

    // Initialize and register a Logger,
    // formatter and flush level will be set according the global settings.
    //
    // Useful for initializing manually created loggers with the global settings.
    //
    // Example:
    //   auto mylogger = rsl::make_shared<rexlog::Logger>("mylogger", ...);
    //   rexlog::initialize_logger(mylogger);
    void initialize_logger(rsl::shared_ptr<Logger> logger);

    // Register the given Logger with the given name
    void register_logger(rsl::shared_ptr<Logger> logger);

    // Return an existing Logger or nullptr if a Logger with such name doesn't
    // exist.
    // example: rexlog::get("my_logger")->info("hello {}", "world");
    rsl::shared_ptr<Logger> get(rsl::string_view name);

    // Set global formatter. Each sink in each Logger will get a clone of this object
    void set_formatter(PatternFormatter&& formatter);

    // Set global format string.
    // example: rexlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
    void set_pattern(const rsl::small_stack_string& pattern, PatternTimeType timeType = PatternTimeType::Local);

    // Set global logging level
    void set_global_level(level::LevelEnum logLevel);

    // Get global logging level
    level::LevelEnum global_level();

    // Set global flush level
    void flush_on(level::LevelEnum logLevel);

    // Flush all loggers
    void flush_all();

    // stop any running threads started by rexlog and clean Registry loggers
    void shutdown();

} // namespace rexlog
