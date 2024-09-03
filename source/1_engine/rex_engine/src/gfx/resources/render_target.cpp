#include "rex_engine/gfx/resources/render_target.h"

namespace rex
{
  namespace gfx
  {
    RenderTarget::RenderTarget(s32 width, s32 height, const ClearStateDesc& clearState)
      : m_width(width)
      , m_height(height)
      , m_clear_state(clearState)
    {}

    s32 RenderTarget::width() const
    {
      return m_width;
    }
    s32 RenderTarget::height() const
    {
      return m_height;
    }
    const ClearStateDesc& RenderTarget::clear_state() const
    {
      return m_clear_state;
    }
  }
}