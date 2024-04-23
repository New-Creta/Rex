#pragma once

#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_std/bonus/math/color.h"

namespace rex
{
  namespace renderer
  {
    struct CreateClearStateDesc
    {
      CreateClearStateDesc()
          : rgba(1.0f, 1.0f, 1.0f, 1.0f)
          , depth(1.0f)
          , stencil(0x00)
          , flags()
      {
        flags.add_state(renderer::ClearBits::ClearColorBuffer);
        flags.add_state(renderer::ClearBits::ClearDepthBuffer);
      }

      rsl::Color4f rgba;
      f32 depth;
      u8 stencil;
      StateController<ClearBits> flags {};
    };
  } // namespace renderer
} // namespace rex