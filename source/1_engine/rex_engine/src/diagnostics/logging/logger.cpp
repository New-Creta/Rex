#include "logging/logger.h"

#include <rexlog/sinks/stdout_color_sinks.h>
#include <rexlog/sinks/basic_file_sink.h>

#include <rex_std/filesystem.h>
#include <rex_std/unordered_map.h>
#include <rex_std/vector.h>
#include <rex_std/utility.h>

namespace rex
{
    using LogPattern = const char*;
    using LogLevelMap = rsl::unordered_map<LogVerbosity, rexlog::level::level_enum>;

    using LoggerObjectPtr = rsl::shared_ptr<rexlog::logger>;
    using LoggerObjectPtrMap = rsl::unordered_map<LogCategoryName, LoggerObjectPtr>;

    LoggerObjectPtrMap  g_loggers;

    //-------------------------------------------------------------------------
    LogLevelMap get_log_levels()
    {
        return
        {
            { LogVerbosity::NoLogging,      rexlog::level::off },
            { LogVerbosity::Fatal,          rexlog::level::critical },
            { LogVerbosity::Error,          rexlog::level::err },
            { LogVerbosity::Warning,        rexlog::level::warn },
            { LogVerbosity::Log,            rexlog::level::info },
            { LogVerbosity::Verbose,        rexlog::level::debug },
            { LogVerbosity::VeryVerbose,    rexlog::level::trace }
        };
    }

    //-------------------------------------------------------------------------
    rexlog::logger* find_logger(const LogCategoryName& name)
    {
        auto it = g_loggers.find(name);

        return it != rsl::cend(g_loggers)
            ? (*it).value.get()
            : nullptr;
    }

    //-------------------------------------------------------------------------
    rexlog::logger& get_logger(const LogCategoryBase& category)
    {
        const LogPattern DEFAULT_PATTERN = "%^[%T][%=8l] %n: %v%$";
        const LogLevelMap LOG_LEVELS = get_log_levels();

        //assert(LOG_LEVELS.find(category.getVerbosity()) != rsl::cend(LOG_LEVELS) && "Unknown log verbosity was given");

        rexlog::logger* logger = find_logger(category.getCategoryName());
        if (logger != nullptr)
            return *logger;

        // rsl::filesystem::path working_dir(rsl::filesystem::current_path());
        // rsl::filesystem::path log_dir("log");
        // rsl::filesystem::path filename(category.getCategoryName().data());
        // rsl::filesystem::path full_path = working_dir / log_dir / filename;

        rsl::vector<rexlog::sink_ptr> sinks;

#if REX_DEBUG
        // Only push rexout color sink when we are in debug mode
        sinks.push_back(rsl::make_shared<rexlog::sinks::stdout_color_sink_mt>());
#endif
        //sinks.push_back(rsl::make_shared<rexlog::sinks::basic_file_sink_mt>(full_path.string(), true));

        rsl::shared_ptr<rexlog::logger> new_logger = rsl::make_shared<rexlog::logger>(category.getCategoryName(), rsl::begin(sinks), rsl::end(sinks));

        new_logger->set_pattern(rsl::string(DEFAULT_PATTERN));
        new_logger->set_level(LOG_LEVELS.at(category.getVerbosity()));

        g_loggers.insert({category.getCategoryName(), new_logger});

        return *find_logger(category.getCategoryName());
    }
}