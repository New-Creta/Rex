#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class RootSignature
    {
    public:
      RootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature);

      ID3D12RootSignature* dx_object();

    private:
      wrl::ComPtr<ID3D12RootSignature> m_root_signature;
    };
  }
}