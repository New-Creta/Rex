#pragma once

#include "rex_renderer_core/gfx/compute_engine.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_compute_context.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    class DxComputeEngine : public gfx::ComputeEngine
    {
    public:
      void init() override;

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(CommandAllocator* alloc) override;

      void new_frame() override;
      void end_frame() override;
    };
  }
}