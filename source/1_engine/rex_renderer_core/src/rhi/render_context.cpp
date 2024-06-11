#include "rex_renderer_core/rhi/render_context.h"

#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
  namespace rhi
  {
    RenderContext::RenderContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, CommandType::Render)
    {
    }

    RenderContext::~RenderContext()
    {
    }

  }
}