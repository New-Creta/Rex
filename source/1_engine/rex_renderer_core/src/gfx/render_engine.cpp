#include "rex_renderer_core/gfx/render_engine.h"

namespace rex
{
  namespace gfx
  {
    RenderEngine::RenderEngine(rhi::ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(rhi::GraphicsEngineType::Render, globalResourceStateTracker)
    {}
  }
}