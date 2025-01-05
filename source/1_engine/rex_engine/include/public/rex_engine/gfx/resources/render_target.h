#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/resources/clear_state.h"

namespace rex
{
  namespace gfx
  {
    // A render target resource, only holding its width and height
    // The actual resource is implemented in an API specific class
    class RenderTarget : public Resource
    {
    public:
      RenderTarget(s32 width, s32 height, const ClearStateDesc& clearState);
      virtual ~RenderTarget() = default;

      s32 width() const;
      s32 height() const;
      const ClearStateDesc& clear_state() const;

    private:
      s32 m_width;
      s32 m_height;
      ClearStateDesc m_clear_state;
    };
  }
}