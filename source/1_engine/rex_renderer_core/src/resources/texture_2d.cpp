#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
  namespace rhi
  {
    Texture2D::Texture2D(s32 width, s32 height, renderer::VertexBufferFormat format)
      : m_width(width)
      , m_height(height)
      , m_format(format)
    {}

    s32 Texture2D::width() const
    {
      return m_width;
    }
    s32 Texture2D::height() const
    {
      return m_height;
    }
    renderer::VertexBufferFormat Texture2D::format() const
    {
      return m_format;
    }
  }
}