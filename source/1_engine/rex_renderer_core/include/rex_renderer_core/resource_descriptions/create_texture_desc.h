#pragma once

#include "rex_renderer_core/resource_descriptions/create_clear_state_desc.h"
#include "rex_renderer_core/resource_descriptions/create_resource_desc.h"
#include "rex_renderer_core/format.h"

namespace rex
{
  namespace renderer
  {
    struct CreateTextureDesc
    {
      CreateResourceDesc resource_desc       = {};
      CreateClearStateDesc* clear_state_desc = nullptr;
      Format clear_state_format              = Format::UNKNOWN;
    };
  } // namespace renderer
} // namespace rex