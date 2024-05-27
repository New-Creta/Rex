#include "rex_directx/system/dx_root_signature.h"

namespace rex
{
  namespace rhi
  {
    RootSignature::RootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature)
      : m_root_signature(rootSignature)
    {}

    ID3D12RootSignature* RootSignature::dx_object()
    {
      return m_root_signature.Get();
    }
  }
}