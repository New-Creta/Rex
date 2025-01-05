#pragma once

#include "rex_engine/engine/state_controller.h"
#include "rex_engine/gfx/core/clear_bits.h"
#include "rex_std/bonus/math/color.h"

namespace rex
{
  namespace gfx
  {
    struct ClearStateDesc
    {
      ClearStateDesc()
          : rgba(1.0f, 1.0f, 1.0f, 1.0f)
          , depth(1.0f)
          , stencil(0x00)
          , flags()
      {
      }

      rsl::Color4f rgba;
      f32 depth;
      u8 stencil;
      StateController<ClearBits> flags {};
    };

    ClearStateDesc default_rtv_clear_state();
    ClearStateDesc default_depth_clear_state();
    ClearStateDesc default_stencil_clear_state();
    ClearStateDesc default_dsv_clear_state();
  } // namespace gfx
} // namespace rex