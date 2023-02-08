#pragma once

#include "rex_engine/types.h"
#include "rex_engine/cmd_line_args.h"

namespace rex
{
  struct PlatformCreationParams;
  s32 app_entry(const PlatformCreationParams& platformParams, CommandLineArguments&& cmdArgs);
} // namespace rex