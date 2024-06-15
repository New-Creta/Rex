#pragma once

#include "rex_renderer_core/system/graphics_engine.h"
#include "rex_renderer_core/gfx/copy_context.h"

namespace rex
{
  namespace gfx
  {
    // The copy engine is responsible for all copy commands on the gpu
    class CopyEngine : public GraphicsEngine
    {
    public:
      CopyEngine(ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}