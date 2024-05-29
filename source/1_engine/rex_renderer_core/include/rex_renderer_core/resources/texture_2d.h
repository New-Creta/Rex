#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rhi/texture_format.h"

namespace rex
{
  namespace rhi
  {
    class Texture2D
    {
    public:
      Texture2D(s32 width, s32 height, renderer::VertexBufferFormat format);

      s32 width() const;
      s32 height() const;

    private:
      s32 m_width;
      s32 m_height;
      renderer::VertexBufferFormat m_format;
    };
  }
}