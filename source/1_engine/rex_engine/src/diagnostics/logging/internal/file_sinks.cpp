

#ifndef REXLOG_COMPILED_LIB
#    error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <rexlog/details/null_mutex.h>
#include <rexlog/details/file_helper-inl.h>
#include <rexlog/sinks/basic_file_sink-inl.h>
#include <rexlog/sinks/base_sink-inl.h>

#include <mutex>

template class REXLOG_API rexlog::sinks::basic_file_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::basic_file_sink<rexlog::details::null_mutex>;

#include <rexlog/sinks/rotating_file_sink-inl.h>
template class REXLOG_API rexlog::sinks::rotating_file_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::rotating_file_sink<rexlog::details::null_mutex>;
