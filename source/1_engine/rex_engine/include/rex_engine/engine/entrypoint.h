#pragma once

#include "rex_std/bonus/types.h"
#include "rex_engine/engine_params.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;


  extern ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams);

  namespace internal
  {
    void pre_app_entry(const char8* cmdLine);
    void post_app_shutdown();
  } // namespace internal
} // namespace rex