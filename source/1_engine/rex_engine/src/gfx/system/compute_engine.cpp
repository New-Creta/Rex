#include "rex_engine/gfx/system/compute_engine.h"

namespace rex
{
  namespace gfx
  {
    ComputeEngine::ComputeEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(gpuEngine, GraphicsEngineType::Compute, globalResourceStateTracker)
    {}
  }
}