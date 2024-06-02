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

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) override
      {
        wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Compute);

        return rsl::make_unique<rhi::DxComputeContext>(this, cmdlist, alloc);
      }

    };
  }
}