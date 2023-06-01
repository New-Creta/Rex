#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"

namespace rexlog
{
    namespace details
    {
        PaddingInfo::PaddingInfo()
            :width(0)
            ,side(PadSide::Left)
            ,truncate(false)
            ,enabled(false)
        {
            // Nothing to implement
        }

        PaddingInfo::PaddingInfo(card32 width, PaddingInfo::PadSide side, bool truncate)
            : width(width)
            , side(side)
            , truncate(truncate)
            , enabled(true)
        {
            // Nothing to implement
        }
    }
}