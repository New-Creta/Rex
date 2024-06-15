#pragma once

#include "rex_engine/engine/state_controller.h"
#include "rex_renderer_core/gfx/clear_bits.h"
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

    // A resource representing which buffers to clear and how to clear them
    class ClearState
    {
    public:
      explicit ClearState(const ClearStateDesc& cs)
        : m_clear_state(cs)
      {}

      ClearStateDesc* get()
      {
        return &m_clear_state;
      }

    private:
      ClearStateDesc m_clear_state;
    };
  } // namespace gfx
} // namespace rex