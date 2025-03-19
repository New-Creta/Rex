#pragma once

#include "rex_engine/gfx/system/render_engine.h"
#include "rex_directx/gfx/dx_render_context.h"
#include "rex_directx/system/dx_gal.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS

namespace rex
{
  namespace gfx
  {
    class DxRenderEngine : public gfx::RenderEngine
    {
    public:
      DxRenderEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker);

      // Initialize all the resources required for the compute engine
      void init() override;

      // Prepare a new frame
      void new_frame() override;
      // Finalise the frame
      void end_frame() override;

    protected:
      // Allocate a render context
      rsl::unique_ptr<GraphicsContext> allocate_new_context(CommandAllocator* alloc) override;
    };
  }
}