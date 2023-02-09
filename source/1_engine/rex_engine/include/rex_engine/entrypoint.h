#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/types.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;

  ApplicationCreationParams app_entry(const PlatformCreationParams& platformParams, const CommandLineArguments& cmdArgs);
} // namespace rex