#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  struct ConsoleApplicationCreationParams;
  class CommandLineArguments;

  void pre_app_entry(const char8* cmdLine);
  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs);
} // namespace rex