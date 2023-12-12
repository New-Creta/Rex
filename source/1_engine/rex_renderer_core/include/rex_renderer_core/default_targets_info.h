#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/resource_slot.h"

namespace rex
{
    struct DefaultTargetsInfo
    {
        renderer::ResourceSlot front_buffer_color = renderer::ResourceSlot::make_invalid();
        renderer::ResourceSlot back_buffer_color = renderer::ResourceSlot::make_invalid();
        renderer::ResourceSlot depth_buffer = renderer::ResourceSlot::make_invalid();
    };
}