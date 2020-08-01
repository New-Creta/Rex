#pragma once

#ifdef _DEBUG

//-------------------------------------------------------------------------
// Includes
#include "rex_diagnostics_defines.h"

#include "logger.h"

#include <assert.h>

//-------------------------------------------------------------------------
// Assembly instruction to break execution.
#ifdef x64
    #define DEBUG_BREAK __debugbreak()
#else
    #error DEBUG_BREAK unsupported machine instruction ...
#endif

//-------------------------------------------------------------------------
// Rex Logger Names
#if defined (REX_CORE_LIB) || defined (REX_DIAGNOSTICS_LIB) || defined (REX_PLATFORM_LIB) || defined (REX_GRAPHICS_LIB) || defined (REX_ENGINE_LIB)
    #define REX_LOGGER_NAME rex::Logger::ENGINE_LOGGER_NAME
#else
    #define REX_LOGGER_NAME rex::Logger::CLIENT_LOGGER_NAME
#endif

//-------------------------------------------------------------------------
// Logging features
#define RX_FATAL(...)   rex::Logger::getLogger(REX_LOGGER_NAME).critical(__VA_ARGS__)
#define RX_ERROR(...)   rex::Logger::getLogger(REX_LOGGER_NAME).error(__VA_ARGS__)
#define RX_WARN(...)    rex::Logger::getLogger(REX_LOGGER_NAME).warn(__VA_ARGS__)
#define RX_INFO(...)    rex::Logger::getLogger(REX_LOGGER_NAME).info(__VA_ARGS__)
#define RX_TODO(...)    rex::Logger::getLogger(REX_LOGGER_NAME).debug(__VA_ARGS__)
#define RX_TRACE(...)   rex::Logger::getLogger(REX_LOGGER_NAME).trace(__VA_ARGS__)

//-------------------------------------------------------------------------
// Assertions
#define RX_ASSERT_X(expr, ...)                          \
    if(expr)                                            \
    {}                                                  \
    else                                                \
    {                                                   \
       RX_FATAL("Assertion Failed: {0}", __VA_ARGS__);  \
       DEBUG_BREAK;                                     \
    }
#define RX_ASSERT(expr)                                 \
    if(expr)                                            \
    {}                                                  \
    else                                                \
    {                                                   \
       RX_FATAL("Assertion Failed: {0}", #expr);        \
       DEBUG_BREAK;                                     \
    }

//-------------------------------------------------------------------------
// Initialize & shutdown the logger
#define RX_INITIALIZE_LOGGER() assert(rex::Logger::initialize(REX_LOGLEVEL) && "Logger was not initialized!")
#define RX_SHUTDOWN_LOGGER() assert(rex::Logger::shutdown() && "Failed to shutdown logger")

#else

#define RX_FATAL(...) UNUSED_PARAM(__VA_ARGS__)
#define RX_ERROR(...) UNUSED_PARAM(__VA_ARGS__)
#define RX_WARN(...)  UNUSED_PARAM(__VA_ARGS__)
#define RX_INFO(...)  UNUSED_PARAM(__VA_ARGS__)
#define RX_TODO(...)  UNUSED_PARAM(__VA_ARGS__)
#define RX_TRACE(...) UNUSED_PARAM(__VA_ARGS__)

#define RX_ASSERT_X(x, ...) {auto check = x; UNUSED_PARAM(check);}
#define RX_ASSERT(...) 

#define RX_INITIALIZE_LOGGER()
#define RX_SHUTDOWN_LOGGER()

#endif