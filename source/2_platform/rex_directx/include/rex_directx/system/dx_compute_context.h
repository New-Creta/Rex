#pragma once

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/rhi/compute_context.h"
#include "rex_renderer_core/gfx/graphics_engine.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class DxComputeContext : public ComputeContext
    {
    public:
      DxComputeContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc);

      ID3D12GraphicsCommandList* dx_cmdlist();

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}