#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  enum class ApplicationState
  {
    Invalid          = BIT(0),  
    Created          = BIT(1), // Application is created but not yet initialized
    Initializing     = BIT(2), // Application is currently being initialized
    Running          = BIT(3), // Application is fully initialized and running
    Paused           = BIT(4), // Application is paused
    MarkedForDestroy = BIT(5), // Application is marked for destruction on the next tick
    ShuttingDown     = BIT(6)  // Application is currently being destroyed
  };
} // namespace rex