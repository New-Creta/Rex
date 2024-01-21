#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility/yes_no.h"

namespace rex
{
  namespace log
  {
    namespace details
    {
      DEFINE_YES_NO_ENUM(Truncate);
      DEFINE_YES_NO_ENUM(Enabled);

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
    }

  } // namespace details
} // namespace rex::log