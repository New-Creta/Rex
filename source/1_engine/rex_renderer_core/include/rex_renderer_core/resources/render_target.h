#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resources/resource.h"

namespace rex
{
  namespace rhi
  {
    class RenderTarget : public Resource
    {
    public:
      RenderTarget(s32 width, s32 height);
      virtual ~RenderTarget() = default;

      s32 width() const;
      s32 height() const;

    private:
      s32 m_width;
      s32 m_height;
    };
  }
}