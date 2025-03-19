#include "rex_engine/gfx/system/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyEngine::CopyEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(gpuEngine, GraphicsEngineType::Copy, globalResourceStateTracker)
    {}
  }
}