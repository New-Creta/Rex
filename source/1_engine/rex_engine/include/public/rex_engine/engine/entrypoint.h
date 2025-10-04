#pragma once

#include "rex_engine/engine/engine_params.h"
#include "rex_std/bonus/types.h"


namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;

  // This acts as the user's entry point to configure engine initialization
  // title, window creation, user initialization and update is all configured here
  extern ApplicationCreationParams app_entry(PlatformCreationParams& platformParams);

  namespace internal
  {
    // function called before the app's entry point (as the name implies)
    // this is do very early initialization and boot flow logic (like debugger attachment on boot etc)
    void pre_app_entry(const char8* cmdLine);

    // very late shutdown code. clients should not rely on this code
    void post_app_shutdown();
  } // namespace internal
} // namespace rex