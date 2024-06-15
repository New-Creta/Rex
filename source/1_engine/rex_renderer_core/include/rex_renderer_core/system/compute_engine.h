#pragma once

#include "rex_renderer_core/system/graphics_engine.h"
#include "rex_renderer_core/gfx/compute_context.h"

namespace rex
{
  namespace gfx
  {
    // The compute engine is responsible for all compute commands on the gpu
    class ComputeEngine : public GraphicsEngine
    {
    public:
      ComputeEngine(ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}