#pragma once

#include "rex_diagnostics_global.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace rex
{
    enum class LogLevel;

    class Logger
    {
    public:
        REX_DIAGNOSTICS_EXPORT Logger() = delete;

        REX_DIAGNOSTICS_EXPORT static const std::string ENGINE_LOGGER_NAME;
        REX_DIAGNOSTICS_EXPORT static const std::string CLIENT_LOGGER_NAME;

        REX_DIAGNOSTICS_EXPORT static bool initialize(const std::string& loglevel);
        REX_DIAGNOSTICS_EXPORT static bool shutdown();

        REX_DIAGNOSTICS_EXPORT static bool addLogger(const std::string& name);
        REX_DIAGNOSTICS_EXPORT static bool addLogger(const std::string& name, const std::string& pattern);
        REX_DIAGNOSTICS_EXPORT static bool addLogger(const std::string& name, const std::string& pattern, rex::LogLevel level);

        REX_DIAGNOSTICS_EXPORT static spdlog::logger& getLogger(const std::string& name);

    private:
        static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> s_loggers;
    };
}