#pragma once

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
  class CommandLineArguments;

  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs);
} // namespace rex