#pragma once

#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    enum class ClearBits
    {
      None               = BIT(0),
      ClearColorBuffer   = BIT(1),
      ClearDepthBuffer   = BIT(2),
      ClearStencilBuffer = BIT(3),
    };

    bool operator&(ClearBits bits1, ClearBits bits2);
    bool operator|(ClearBits bits1, ClearBits bits2);
  } // namespace renderer
} // namespace rex