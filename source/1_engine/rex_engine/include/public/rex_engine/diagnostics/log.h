#pragma once

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_std/string.h"

DEFINE_LOG_CATEGORY(LogEngine);

namespace rex
{
	rsl::string log_path();
}