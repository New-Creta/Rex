#pragma once

#if REX_PLATFORM_WINDOWS
#include "rex_engine/diagnostics/logging/internal/sinks/win/wincolor_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/win/winstdout_color_sinks.h"
#else
#include "rex_engine/diagnostics/logging/internal/sinks/unix/ansicolor_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/unix/unixstdout_color_sinks.h"
#endif