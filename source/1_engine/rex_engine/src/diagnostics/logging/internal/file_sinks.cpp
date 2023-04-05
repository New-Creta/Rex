

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include "rex_engine/diagnostics/logging/internal/details/FileHelper-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/BaseSink-inl.h"
#include "rex_engine/diagnostics/logging/internal/sinks/BasicFileSink-inl.h"

template class REXLOG_API rexlog::sinks::BasicFileSink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::BasicFileSink<rexlog::details::NullMutex>;

#include "rex_engine/diagnostics/logging/internal/sinks/rotating_file_sink-inl.h"
template class REXLOG_API rexlog::sinks::rotating_file_sink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::rotating_file_sink<rexlog::details::NullMutex>;
