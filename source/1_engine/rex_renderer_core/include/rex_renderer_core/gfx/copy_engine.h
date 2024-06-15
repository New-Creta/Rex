#pragma once

#include "rex_renderer_core/gfx/graphics_engine.h"
#include "rex_renderer_core/rhi/copy_context.h"

namespace rex
{
  namespace gfx
  {
    // The copy engine is responsible for all copy commands on the gpu
    class CopyEngine : public GraphicsEngine
    {
    public:
      CopyEngine(rhi::ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}