#pragma once

#include "rex_std/bonus/types.h"
#include "rex_engine/cmdline.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  struct ConsoleApplicationCreationParams;

  void pre_app_entry(const char8* cmdLine);
  cmdline::CommandLineArgumentsView receive_engine_cmd_args();

  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams);
  void post_app_shutdown();
} // namespace rex