#pragma once

#include "rex_engine/defines.h"

namespace rex
{
  enum class ApplicationState
  {
    Invalid          = BIT(0),
    Initializing     = BIT(1),
    Running          = BIT(2),
    Paused           = BIT(3),
    MarkedForDestroy = BIT(4),
    ShuttingDown     = BIT(5)
  };
} // namespace rex