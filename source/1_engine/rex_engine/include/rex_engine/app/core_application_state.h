#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  enum class ApplicationState
  {
    Invalid          = BIT(0),
    Created          = BIT(1),
    Initializing     = BIT(2),
    Running          = BIT(3),
    Paused           = BIT(4),
    MarkedForDestroy = BIT(5),
    ShuttingDown     = BIT(6)
  };
} // namespace rex