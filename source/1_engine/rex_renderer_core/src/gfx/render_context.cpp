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
    // Reset the engine specific context
    void RenderContext::type_specific_reset(const ContextResetData& resetData)
    {
      // By default we set the current backbuffer as render target
      set_render_target(resetData.current_backbuffer_rt);
    }
  }
}