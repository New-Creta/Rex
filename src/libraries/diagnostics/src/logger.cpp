#include "logger.h"

#ifdef _WINDOWS
    #undef ERROR    // Defined in wingdi.h
#endif

namespace
{
    //-------------------------------------------------------------------------
    const std::string DEFAULT_PATTERN = ("%^[%T] %n: %v%$");

    //-------------------------------------------------------------------------
    const std::unordered_map<rex::LogLevel, spdlog::level::level_enum> LOG_LEVELS =
    {
        {rex::LogLevel::OFF,    spdlog::level::off},
        {rex::LogLevel::FATAL,  spdlog::level::critical},
        {rex::LogLevel::ERROR,  spdlog::level::err},
        {rex::LogLevel::WARN,   spdlog::level::warn},
        {rex::LogLevel::INFO,   spdlog::level::info},
        {rex::LogLevel::DEBUG,  spdlog::level::debug},
        {rex::LogLevel::TRACE,  spdlog::level::trace}
    };

    //-------------------------------------------------------------------------
    spdlog::logger* findLogger(const std::unordered_map<std::string, std::shared_ptr<spdlog::logger>>& loggers, const std::string& name)
    {
        auto it = loggers.find(name);

        return it != std::cend(loggers)
            ? (*it).second.get()
            : nullptr;
    }
}

//-------------------------------------------------------------------------
const std::string rex::Logger::ENGINE_LOGGER_NAME = "REX";
//-------------------------------------------------------------------------
const std::string rex::Logger::CLIENT_LOGGER_NAME = "APP";

//-------------------------------------------------------------------------
std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> rex::Logger::s_loggers = {};

//-------------------------------------------------------------------------
bool rex::Logger::initialize(const std::string& logLevel)
{
    const std::unordered_map<std::string, rex::LogLevel> levels =
    {
        {   "Off",     rex::LogLevel::OFF      },
        {   "Fatal",   rex::LogLevel::FATAL    },
        {   "Error",   rex::LogLevel::ERROR    },
        {   "Warning", rex::LogLevel::WARN     },
        {   "Info",    rex::LogLevel::INFO     },
        {   "Debug",   rex::LogLevel::DEBUG    },
        {   "Trace",   rex::LogLevel::TRACE    },
    };

    if (levels.find(logLevel) == std::cend(levels))
        return false;

    bool result = true;

    result &= addLogger(rex::Logger::ENGINE_LOGGER_NAME, DEFAULT_PATTERN, levels.at(logLevel));
    result &= addLogger(rex::Logger::CLIENT_LOGGER_NAME, DEFAULT_PATTERN, levels.at(logLevel));

    return result;
}
//-------------------------------------------------------------------------
bool rex::Logger::shutdown()
{
    s_loggers.clear();

    return true;
}

//-------------------------------------------------------------------------
bool rex::Logger::addLogger(const std::string& name)
{
    return addLogger(name, DEFAULT_PATTERN, rex::LogLevel::TRACE);
}
//-------------------------------------------------------------------------
bool rex::Logger::addLogger(const std::string& name, const std::string& pattern)
{
    return addLogger(name, pattern, rex::LogLevel::TRACE);
}
//-------------------------------------------------------------------------
bool rex::Logger::addLogger(const std::string& name, const std::string& pattern, rex::LogLevel level)
{
    assert(LOG_LEVELS.find(level) != std::cend(LOG_LEVELS) && "Unknown log level was given");

    spdlog::logger* logger = findLogger(s_loggers, name);
    if (logger != nullptr)
        return false;

    std::shared_ptr<spdlog::logger> new_logger = spdlog::stdout_color_mt(name);

    new_logger->set_pattern(pattern);
    new_logger->set_level(LOG_LEVELS.at(level));

    s_loggers.insert(std::make_pair(name, new_logger));

    return true;
}

//-------------------------------------------------------------------------
spdlog::logger& rex::Logger::getLogger(const std::string& name)
{
    #if !defined (REX_CORE_LIB) && !defined (REX_DIAGNOSTICS_LIB)
        assert(name != ENGINE_LOGGER_NAME && "The \"Engine\", logger is for internal use only");
    #endif

    return *findLogger(s_loggers, name);
}