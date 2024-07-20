#pragma once

#include "rex_std/unordered_map.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/resources/root_signature.h"

namespace rex
{
  namespace gfx
  {
    struct ShaderParameter
    {
      s32 textures_root_param_idx;
      s32 samplers_root_param_idx;
    };

    class DxRootSignature : public RootSignature
    {
    public:
      DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature, rsl::vector<CD3DX12_ROOT_PARAMETER>&& rootParameters);

      // Return the parameter index in the root signature for the textures of the shader
      s32 param_idx_for_textures(ShaderType type) override;
      // Return the parameter index in the root signature for the samplers of the shader
      s32 param_idx_for_samplers(ShaderType type) override;

      // Return the wrapped DirectX object
      ID3D12RootSignature* dx_object();

    private:
      wrl::ComPtr<ID3D12RootSignature> m_root_signature;
      rsl::vector<CD3DX12_ROOT_PARAMETER> m_root_parameters;
      rsl::unordered_map<ShaderType, ShaderParameter> m_shader_parameter_idx_cache;
    };
  }
}