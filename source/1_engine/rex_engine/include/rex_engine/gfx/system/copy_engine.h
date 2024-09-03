#pragma once

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/system/copy_context.h"

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