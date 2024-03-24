#pragma once

#include "rex_renderer_core/rendering/clear_bits.h"
#include "rex_std/bonus/math/color.h"

#include "rex_engine/engine/state_controller.h"

namespace rex
{
  namespace rhi
  {
    struct ClearStateDesc
    {
      ClearStateDesc()
        : rgba(1.0f, 1.0f, 1.0f, 1.0f)
        , depth(1.0f)
        , stencil(0x00)
        , flags()
      {}

      rsl::Color4f rgba;
      f32 depth;
      u8 stencil;
      StateController<renderer::ClearBits> flags{};

    };
  }
}