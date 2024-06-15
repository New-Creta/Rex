#include "rex_renderer_core/system/render_engine.h"

namespace rex
{
  namespace gfx
  {
    RenderEngine::RenderEngine(ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(GraphicsEngineType::Render, globalResourceStateTracker)
    {}
  }
}