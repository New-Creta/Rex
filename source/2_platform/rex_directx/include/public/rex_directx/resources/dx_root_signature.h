#pragma once

#include "rex_std/unordered_map.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/gfx/resources/root_signature.h"

namespace rex
{
  namespace gfx
  {
    class DxRootSignature : public RootSignature
    {
    public:
      DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature);

      // Return the wrapped DirectX object
      ID3D12RootSignature* dx_object();

    private:
      wrl::ComPtr<ID3D12RootSignature> m_root_signature;
    };
  }
}