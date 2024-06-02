#include "rex_directx/system/dx_copy_context.h"

namespace rex
{
  namespace rhi
  {
    DxCopyContext::DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc)
      : m_cmd_list(cmdList)
    {

    }

    ID3D12GraphicsCommandList* DxCopyContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

  }
}