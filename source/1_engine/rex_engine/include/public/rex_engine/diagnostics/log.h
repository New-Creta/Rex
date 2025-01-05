#pragma once

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/string/tmp_string.h"

DEFINE_LOG_CATEGORY(LogEngine);

namespace rex
{
	TempString log_path();
}