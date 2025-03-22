#pragma once

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/string/tmp_string.h"

DEFINE_LOG_CATEGORY(LogEngine);

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: LOG PROJECT PATH

namespace rex
{
	scratch_string project_log_path();
}