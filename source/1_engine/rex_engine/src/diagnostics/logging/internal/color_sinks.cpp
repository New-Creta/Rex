

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include "rex_engine/diagnostics/logging/internal/async.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

// IWYU pragma: no_include <built-in>

//
// color sinks
//
#ifdef _WIN32
  #include "rex_engine/diagnostics/logging/internal/sinks/wincolor_sink-inl.h"
template class REXLOG_API rexlog::sinks::WincolorSink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::WincolorSink<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::WincolorStdoutSink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::WincolorStdoutSink<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::WincolorStderrSink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::WincolorStderrSink<rexlog::details::ConsoleNullMutex>;
#else
  #include "rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink-inl.h"
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::ConsoleNullMutex>;
#endif

// factory methods for color loggers
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks-inl.h"
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_color_mt<rexlog::SynchronousFactory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_color_st<rexlog::SynchronousFactory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_color_mt<rexlog::SynchronousFactory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_color_st<rexlog::SynchronousFactory>(const rex::DebugString& logger_name, ColorMode mode);

template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_color_mt<rexlog::async_factory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_color_st<rexlog::async_factory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_color_mt<rexlog::async_factory>(const rex::DebugString& logger_name, ColorMode mode);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_color_st<rexlog::async_factory>(const rex::DebugString& logger_name, ColorMode mode);
