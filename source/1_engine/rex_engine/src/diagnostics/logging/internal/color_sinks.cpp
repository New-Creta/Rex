

#ifndef REXLOG_COMPILED_LIB
#    error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>

#include <rexlog/details/null_mutex.h>
#include <rexlog/async.h>
//
// color sinks
//
#ifdef _WIN32
#    include <rexlog/sinks/wincolor_sink-inl.h>
template class REXLOG_API rexlog::sinks::wincolor_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::wincolor_stdout_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_stdout_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::wincolor_stderr_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::wincolor_stderr_sink<rexlog::details::console_nullmutex>;
#else
#    include "rexlog/sinks/ansicolor_sink-inl.h"
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stdout_sink<rexlog::details::console_nullmutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::console_mutex>;
template class REXLOG_API rexlog::sinks::ansicolor_stderr_sink<rexlog::details::console_nullmutex>;
#endif

// factory methods for color loggers
#include "rexlog/sinks/stdout_color_sinks-inl.h"
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_mt<rexlog::synchronous_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_st<rexlog::synchronous_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_mt<rexlog::synchronous_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_st<rexlog::synchronous_factory>(
    const rsl::string &logger_name, color_mode mode);

template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_mt<rexlog::async_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stdout_color_st<rexlog::async_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_mt<rexlog::async_factory>(
    const rsl::string &logger_name, color_mode mode);
template REXLOG_API rsl::shared_ptr<rexlog::logger> rexlog::stderr_color_st<rexlog::async_factory>(
    const rsl::string &logger_name, color_mode mode);
