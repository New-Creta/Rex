#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
  Texture2D::Texture2D(s32 width, s32 height)
    : m_width(width)
    , m_height(height)
  {}

  s32 Texture2D::width() const
  {
    return m_width;
  }
  s32 Texture2D::height() const
  {
    return m_height;
  }
}