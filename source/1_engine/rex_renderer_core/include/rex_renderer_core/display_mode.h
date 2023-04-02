#pragma once

#include "rex_engine/types.h"
#include "rex_renderer_core/format_type.h"

namespace rex
{
    class Format;

    struct DisplayMode
    {
        DisplayMode()
            : width(0)
            , height(0)
            , format(FormatType::UNKNOWN)
            , refresh_rate()
        {}

        u32 width;
        u32 height;

        FormatType format;

        u32 refresh_rate;
    };
}