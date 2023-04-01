

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/async.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
//
// color sinks
//
#ifdef _WIN32
  #include <rex_engine/diagnostics/logging/internal/sinks/wincolor_sink-inl.h>
template class REXLOG_API rexlog::sinks::wincolor_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::wincolor_stdout_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_stdout_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::wincolor_stderr_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_stderr_sink<rexlog::details::console_nullmutex>;
#else
  #include "rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink-inl.h"
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::console_nullmutex>;
#endif

// factory methods for color loggers
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks-inl.h"
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_mt<rexlog::synchronous_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_st<rexlog::synchronous_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_mt<rexlog::synchronous_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_st<rexlog::synchronous_factory>(const rex::DebugString& logger_name, color_mode mode);

template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_mt<rexlog::async_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_st<rexlog::async_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_mt<rexlog::async_factory>(const rex::DebugString& logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_st<rexlog::async_factory>(const rex::DebugString& logger_name, color_mode mode);
