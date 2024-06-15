#include "rex_renderer_core/gfx/render_context.h"

#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
  namespace gfx
  {
    RenderContext::RenderContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Render)
    {
    }

  }
}