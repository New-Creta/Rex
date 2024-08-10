#include "rex_directx/resources/dx_root_signature.h"

namespace rex
{
  namespace gfx
  {
    DxRootSignature::DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature)
      : m_root_signature(rootSignature)
    {}

    // Return the wrapped DirectX object
    ID3D12RootSignature* DxRootSignature::dx_object()
    {
      return m_root_signature.Get();
    }
  }
}