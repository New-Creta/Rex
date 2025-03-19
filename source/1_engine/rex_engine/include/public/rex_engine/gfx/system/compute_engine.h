#pragma once

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/system/compute_context.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS

namespace rex
{
  namespace gfx
  {
    // The compute engine is responsible for all compute commands on the gpu
    class ComputeEngine : public GraphicsEngine
    {
    public:
      ComputeEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}