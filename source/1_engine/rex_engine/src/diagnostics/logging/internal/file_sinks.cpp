#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

// IWYU pragma: no_include <built-in>

#include "rex_engine/diagnostics/logging/internal/details/file_helper-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink-inl.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink-inl.h"
#include "rex_std/assert.h"
#include "rex_std/mutex.h"

template class REXLOG_API rexlog::sinks::BasicFileSink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::BasicFileSink<rexlog::details::NullMutex>;

#include "rex_engine/diagnostics/logging/internal/sinks/rotating_file_sink-inl.h"

template class REXLOG_API rexlog::sinks::RotatingFileSink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::RotatingFileSink<rexlog::details::NullMutex>;
