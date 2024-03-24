#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource_slot.h"

namespace rex
{
  struct DefaultTargetsInfo
  {
    rhi::ResourceSlot front_buffer_color;
    rhi::ResourceSlot back_buffer_color;
    rhi::ResourceSlot depth_buffer;
  };
} // namespace rex