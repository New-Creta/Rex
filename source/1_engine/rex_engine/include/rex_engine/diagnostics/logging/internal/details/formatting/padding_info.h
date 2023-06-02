#pragma once

#include "rex_engine/types.h"
#include "rex_std_extra/utility/yes_no.h"

DEFINE_YES_NO_ENUM(Truncate);
DEFINE_YES_NO_ENUM(Enabled);

namespace rexlog
{
  namespace details
  {
    struct PaddingInfo
    {
      enum class PadSide
      {
        Left,
        Right,
        Center
      };

      PaddingInfo();
      PaddingInfo(s32 width, PaddingInfo::PadSide side, Truncate truncate);

      s32 width;
      PadSide side;
      Truncate truncate;
      Enabled enabled;
    };
  } // namespace details
} // namespace rexlog