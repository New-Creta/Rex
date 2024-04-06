#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    struct MsaaSettings
    {
      s32 count   = 1; // number of multi samples per pixel
      s32 quality = 0; // the image quality level
    };
  }                    // namespace renderer
} // namespace rex