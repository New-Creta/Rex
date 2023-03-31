

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include <rex_engine/diagnostics/logging/internal/details/file_helper-inl.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink-inl.h>
#include <rex_engine/diagnostics/logging/internal/sinks/basic_file_sink-inl.h>

template class REXLOG_API rexlog::sinks::basic_file_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::basic_file_sink<rexlog::details::null_mutex>;

#include <rex_engine/diagnostics/logging/internal/sinks/rotating_file_sink-inl.h>
template class REXLOG_API rexlog::sinks::rotating_file_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::rotating_file_sink<rexlog::details::null_mutex>;
