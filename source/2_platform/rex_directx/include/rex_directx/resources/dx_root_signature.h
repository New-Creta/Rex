#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/resources/root_signature.h"

namespace rex
{
  namespace rhi
  {
    class DxRootSignature : public RootSignature
    {
    public:
      DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature);

      ID3D12RootSignature* dx_object() const;

    private:
      wrl::ComPtr<ID3D12RootSignature> m_root_signature;
    };
  }
}