

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/async.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/sinks/stdout_sinks-inl.h>

template class REXLOG_API rexlog::sinks::stdout_sink_base<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::stdout_sink_base<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::stdout_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::stdout_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::stderr_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::stderr_sink<rexlog::details::console_nullmutex>;

template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_logger_mt<rexlog::synchronous_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_logger_st<rexlog::synchronous_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_logger_mt<rexlog::synchronous_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_logger_st<rexlog::synchronous_factory>(const rex::DebugString& logger_name);

template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_logger_mt<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_logger_st<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_logger_mt<rexlog::async_factory>(const rex::DebugString& logger_name);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_logger_st<rexlog::async_factory>(const rex::DebugString& logger_name);
