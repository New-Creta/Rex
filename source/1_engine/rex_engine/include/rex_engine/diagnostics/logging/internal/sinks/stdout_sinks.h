#pragma once

#if REX_PLATFORM_WINDOWS
#include "rex_engine/diagnostics/logging/internal/sinks/win/winstdout_sinks.h"
#else
#include "rex_engine/diagnostics/logging/internal/sinks/unix/unixstdout_sinks.h"
#endif