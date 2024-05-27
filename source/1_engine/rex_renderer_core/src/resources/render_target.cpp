#include "rex_renderer_core/resources/render_target.h"

namespace rex
{
  namespace rhi
  {
    RenderTarget::RenderTarget(s32 width, s32 height)
      : m_width(width)
      , m_height(height)
    {}

    s32 RenderTarget::width() const
    {
      return m_width;
    }
    s32 RenderTarget::height() const
    {
      return m_height;
    }
  }
}