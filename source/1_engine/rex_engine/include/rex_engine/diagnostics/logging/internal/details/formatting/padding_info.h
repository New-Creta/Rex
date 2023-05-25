#pragma once

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
            PaddingInfo(size_t width, PaddingInfo::PadSide side, bool truncate);

            size_t      width;
            PadSide     side;
            bool        truncate;
            bool        enabled;
        };
    }
}