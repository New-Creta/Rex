#include "rex_directx/resources/dx_root_signature.h"

namespace rex
{
  namespace gfx
  {
    DxRootSignature::DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature, rsl::vector<CD3DX12_ROOT_PARAMETER>&& rootParameters)
      : m_root_signature(rootSignature)
      , m_root_parameters(rsl::move(rootParameters))
    {}

    s32 DxRootSignature::param_idx_for_textures(ShaderType type)
    {
      return m_shader_parameter_idx.at(type).textures_root_param_idx;
    }
    s32 DxRootSignature::param_idx_for_samplers(ShaderType type)
    {
      return m_shader_parameter_idx.at(type).samplers_root_param_idx;
    }

    ID3D12RootSignature* DxRootSignature::dx_object()
    {
      return m_root_signature.Get();
    }
  }
}