#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class RenderTarget
    {
    public:
      RenderTarget(s32 width, s32 height);

      s32 width() const;
      s32 height() const;

    private:
      s32 m_width;
      s32 m_height;
    };
  }
}