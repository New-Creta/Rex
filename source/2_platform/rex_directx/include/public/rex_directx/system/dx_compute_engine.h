#pragma once

#include "rex_engine/gfx/system/compute_engine.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/gfx/dx_compute_context.h"
#include "rex_directx/system/dx_gal.h"

namespace rex
{
  namespace gfx
  {
    class DxComputeEngine : public gfx::ComputeEngine
    {
    public:
      DxComputeEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker);

      // Initialize all the resources required for the compute engine
      void init() override;

      // Prepare a new frame
      void new_frame() override;
      // Finalise the frame
      void end_frame() override;

    protected:
      // Allocate a copy context
      rsl::unique_ptr<GraphicsContext> allocate_new_context(CommandAllocator* alloc) override;
    };
  }
}