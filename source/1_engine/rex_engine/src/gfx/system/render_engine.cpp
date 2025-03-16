#include "rex_engine/gfx/system/render_engine.h"

namespace rex
{
  namespace gfx
  {
    RenderEngine::RenderEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(gpuEngine, GraphicsEngineType::Render, globalResourceStateTracker)
    {}
  }
}