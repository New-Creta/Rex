#include "rex_renderer_core/gfx/render_engine.h"

namespace rex
{
  namespace gfx
  {
    RenderEngine::RenderEngine()
      : GraphicsEngine(rhi::GraphicsEngineType::Render)
    {}
  }
}