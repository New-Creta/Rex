#pragma once

#include "rex_std/bonus/types.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  struct ConsoleApplicationCreationParams;
  class CommandLineArguments;

  void pre_app_entry(const char8* cmdLine);
  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs);
  void post_app_shutdown();
} // namespace rex