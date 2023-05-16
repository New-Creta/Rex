

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

// IWYU pragma: no_include <built-in>

#include "rex_engine/diagnostics/logging/internal/async.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_sinks-inl.h"

template class REXLOG_API rexlog::sinks::StdoutSinkBase<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::StdoutSinkBase<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::StdoutSink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::StdoutSink<rexlog::details::ConsoleNullMutex>;
template class REXLOG_API rexlog::sinks::StderrSink<rexlog::details::ConsoleMutex>;
template class REXLOG_API rexlog::sinks::StderrSink<rexlog::details::ConsoleNullMutex>;

template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_logger_mt<rexlog::SynchronousFactory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_logger_st<rexlog::SynchronousFactory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_logger_mt<rexlog::SynchronousFactory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_logger_st<rexlog::SynchronousFactory>(const rex::DebugString& logger_name);

template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_logger_mt<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stdout_logger_st<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_logger_mt<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::Logger> rexlog::stderr_logger_st<rexlog::async_factory>(const rex::DebugString& logger_name);
