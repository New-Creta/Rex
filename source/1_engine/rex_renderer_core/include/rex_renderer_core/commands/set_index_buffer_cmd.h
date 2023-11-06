#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/index_buffer_format.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetIndexBuffer
            {
                SetIndexBuffer()
                    :buffer_index(REX_INVALID_INDEX)
                    ,format(IndexBufferFormat::NONE)
                    ,offset(0)
                {}

                s32 buffer_index;
                IndexBufferFormat format;
                s32 offset;
            };
        }
    }
}