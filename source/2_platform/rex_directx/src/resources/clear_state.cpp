#include "rex_directx/resources/clear_state.h"

namespace rex
{
  namespace renderer
  {
    ClearState::ClearState()
        : m_desc({})
    {
    }
    ClearState::ClearState(CreateClearStateDesc&& desc)
        : m_desc(rsl::move(desc))
    {
    }

    ClearState::~ClearState() = default;

    rsl::Color4f ClearState::rgba() const
    {
      return m_desc.rgba;
    }
    f32 ClearState::depth() const
    {
      return m_desc.depth;
    }
    u8 ClearState::stencil() const
    {
      return m_desc.stencil;
    }
    ClearBits ClearState::flags() const
    {
      return m_desc.flags.state();
    }
  } // namespace renderer
} // namespace rex