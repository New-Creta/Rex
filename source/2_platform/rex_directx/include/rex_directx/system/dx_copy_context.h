#pragma once

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/rhi/copy_context.h"
#include "rex_renderer_core/gfx/copy_engine.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator;

    class DxCopyContext : public CopyContext
    {
    public:
      DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc);

      ID3D12GraphicsCommandList* dx_cmdlist();

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;

    };
  }
}