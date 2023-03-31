

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/common-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/backtracer-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/log_msg-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/log_msg_buffer-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/details/os-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/registry-inl.h>
#include <rex_engine/diagnostics/logging/internal/logger-inl.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter-inl.h>
#include <rex_engine/diagnostics/logging/internal/rexlog-inl.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink-inl.h>
#include <rex_engine/diagnostics/logging/internal/sinks/sink-inl.h>

// template instantiate logger constructor with sinks init list
template REXLOG_API rexlog::logger::logger(rsl::string name, sinks_init_list::iterator begin, sinks_init_list::iterator end);
template class REXLOG_API rexlog::sinks::base_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::base_sink<rexlog::details::null_mutex>;
