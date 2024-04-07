#pragma once

#include "rex_engine/engine/engine_params.h"
#include "rex_std/bonus/types.h"
#include "rex_engine/engine/project.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;

  // This acts as the user's entry point to configure engine initialization
  // title, window creation, user initialization and update is all configured here
  extern ApplicationCreationParams app_entry(PlatformCreationParams& platformParams);

  namespace internal
  {
    void pre_app_entry(const char8* cmdLine);
    void post_app_shutdown();
  } // namespace internal
} // namespace rex