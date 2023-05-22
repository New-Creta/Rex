#pragma once

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  struct ConsoleApplicationCreationParams;
  class CommandLineArguments;

  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs);

  namespace internal
  {
    void pre_app_entry(const char8* cmdLine);
    void post_app_shutdown();
  }
} // namespace rex