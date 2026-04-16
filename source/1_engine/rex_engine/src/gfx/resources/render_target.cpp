#include "rex_engine/gfx/resources/render_target.h"

namespace rex
{
  namespace gfx
  {
    RenderTarget::RenderTarget(s32 width, s32 height, ResourceView* resourceView, const rsl::Color4f& clearColor)
      : RenderTargetBase(width, height)
      , m_view(resourceView)
      , m_clear_color(clearColor)
    {}

    const rsl::Color4f& RenderTarget::clear_color() const
    {
      return m_clear_color;
    }

    const ResourceView* RenderTarget::view() const
    {
      return m_view;
    }

  }
}