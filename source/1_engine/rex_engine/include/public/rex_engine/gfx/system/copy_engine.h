#pragma once

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/system/copy_context.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS

namespace rex
{
  namespace gfx
  {
    // The copy engine is responsible for all copy commands on the gpu
    class CopyEngine : public GraphicsEngine
    {
    public:
      CopyEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}