

#ifndef REXLOG_COMPILED_LIB
  #error Please define REXLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>
#include "rex_engine/diagnostics/logging/internal/common-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/Backtracer-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/LogMsg-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/LogMsgBuffer-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/os-inl.h"
#include "rex_engine/diagnostics/logging/internal/details/Registry-inl.h"
#include "rex_engine/diagnostics/logging/internal/logger-inl.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter-inl.h"
#include "rex_engine/diagnostics/logging/internal/rexlog-inl.h"
#include "rex_engine/diagnostics/logging/internal/sinks/BaseSink-inl.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink-inl.h"

// template instantiate logger constructor with sinks init list
template REXLOG_API rexlog::logger::logger(rex::DebugString name, sinks_init_list::iterator begin, sinks_init_list::iterator end);
template class REXLOG_API rexlog::sinks::BaseSink<rsl::mutex>;
template class REXLOG_API rexlog::sinks::BaseSink<rexlog::details::NullMutex>;
