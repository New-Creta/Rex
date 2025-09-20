#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/render_target_base.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/system/resource_view.h"

namespace rex
{
  namespace gfx
  {
    // A render target resource, only holding its width and height
    // The actual resource is implemented in an API specific class
    class RenderTarget : public RenderTargetBase
    {
    public:
      RenderTarget(s32 width, s32 height, ResourceView* resourceView, const rsl::Color4f& clearColor);
      virtual ~RenderTarget() = default;

      const rsl::Color4f& clear_color() const override;

      // Return the resource view of the texture
      const ResourceView* view() const override;

    private:
      rsl::Color4f m_clear_color;
      ResourceView* m_view;
    };
  }
}