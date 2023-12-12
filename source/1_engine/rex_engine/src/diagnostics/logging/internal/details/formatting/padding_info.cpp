#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"

// IWYU pragma: no_include <built-in>

namespace rexlog
{
  namespace details
  {
    PaddingInfo::PaddingInfo()
        : width(0)
        , side(PadSide::Left)
        , truncate(Truncate::no)
        , enabled(Enabled::no)
    {
      // Nothing to implement
    }

    PaddingInfo::PaddingInfo(s32 width, PaddingInfo::PadSide side, Truncate truncate)
        : width(width)
        , side(side)
        , truncate(truncate)
        , enabled(Enabled::yes)
    {
      // Nothing to implement
    }
  } // namespace details
} // namespace rexlog