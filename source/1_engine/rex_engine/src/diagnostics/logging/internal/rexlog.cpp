

#ifndef REXLOG_COMPILED_LIB
#    error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <rexlog/rexlog-inl.h>
#include <rexlog/common-inl.h>
#include <rexlog/details/backtracer-inl.h>
#include <rexlog/details/registry-inl.h>
#include <rexlog/details/os-inl.h>
#include <rexlog/pattern_formatter-inl.h>
#include <rexlog/details/log_msg-inl.h>
#include <rexlog/details/log_msg_buffer-inl.h>
#include <rexlog/logger-inl.h>
#include <rexlog/sinks/sink-inl.h>
#include <rexlog/sinks/base_sink-inl.h>
#include <rexlog/details/null_mutex.h>

#include <mutex>

// template instantiate logger constructor with sinks init list
template REXLOG_API rexlog::logger::logger(rsl::string name, sinks_init_list::iterator begin, sinks_init_list::iterator end);
template class REXLOG_API rexlog::sinks::base_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::base_sink<rexlog::details::null_mutex>;
