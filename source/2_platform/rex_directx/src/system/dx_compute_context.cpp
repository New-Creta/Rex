#include "rex_directx/system/dx_compute_context.h"

namespace rex
{
  namespace rhi
  {
    DxComputeContext::DxComputeContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList)
      : ComputeContext(owningEngine)
      , m_cmd_list(cmdList)
    {

    }

    ID3D12GraphicsCommandList* DxComputeContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

  }
}