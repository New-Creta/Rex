#pragma once

#include "rex_engine/gfx/system/render_engine.h"
#include "rex_directx/gfx/dx_render_context.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace gfx
  {
    class DxRenderEngine : public gfx::RenderEngine
    {
    public:
      DxRenderEngine(ResourceStateTracker* globalResourceStateTracker);

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