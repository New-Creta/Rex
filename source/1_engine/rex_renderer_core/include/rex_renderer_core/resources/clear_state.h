#pragma once

#include "rex_engine/engine/state_controller.h"
#include "rex_renderer_core/rhi/clear_bits.h"
#include "rex_std/bonus/math/color.h"

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
      {
      }

      rsl::Color4f rgba;
      f32 depth;
      u8 stencil;
      StateController<renderer::ClearBits> flags {};
    };

    class ClearStateResource
    {
    public:
      explicit ClearStateResource(const ClearStateDesc& cs)
        : m_clear_state(cs)
      {}

      ClearStateDesc* get()
      {
        return &m_clear_state;
      }

    private:
      ClearStateDesc m_clear_state;
    };
  } // namespace rhi
} // namespace rex