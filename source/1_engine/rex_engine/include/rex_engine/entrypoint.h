#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  class CommandLineArguments;
  
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs);
} // namespace rex