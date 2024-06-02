#pragma once

#include "rex_renderer_core/gfx/copy_engine.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_copy_context.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    class DxCopyEngine : public gfx::CopyEngine
    {
    public:

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) override
      {
        wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Copy);

        return rsl::make_unique<rhi::DxCopyContext>(this, cmdlist, alloc);
      }

    };
  }
}