#include "rex_directx/resources/dx_root_signature.h"

namespace rex
{
  namespace gfx
  {
    DxRootSignature::DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature, const rsl::vector<CD3DX12_ROOT_PARAMETER>& rootParameters)
      : m_root_signature(rootSignature)
      , m_root_parameters(rootParameters)
    {
      for (s32 idx = 0; idx < m_root_parameters.size(); ++idx)
      {
        const auto& param = m_root_parameters[idx];

        // We need to cache the parameter indices for textures and samplers so we can set them when a material gets bound
        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
					ShaderVisibility visibility = d3d::from_dx12(param.ShaderVisibility);
					ShaderType type = d3d::shader_visibility_to_type(visibility);

          if (param.DescriptorTable.pDescriptorRanges[0].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
          {
            m_shader_parameter_idx_cache[type].textures_root_param_idx = idx;
          }

          else if (param.DescriptorTable.pDescriptorRanges[0].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
          {
            m_shader_parameter_idx_cache[type].samplers_root_param_idx = idx;
          }
        }
      }
    }

    // Return the parameter index in the root signature for the textures of the shader
    s32 DxRootSignature::param_idx_for_textures(ShaderType type)
    {
      if (m_shader_parameter_idx_cache.contains(type))
      {
        return m_shader_parameter_idx_cache.at(type).textures_root_param_idx;
      }

      return -1;
    }
    // Return the parameter index in the root signature for the samplers of the shader
    s32 DxRootSignature::param_idx_for_samplers(ShaderType type)
    {
      if (m_shader_parameter_idx_cache.contains(type))
      {
        return m_shader_parameter_idx_cache.at(type).samplers_root_param_idx;
      }

      return -1;
    }

    // Return the wrapped DirectX object
    ID3D12RootSignature* DxRootSignature::dx_object()
    {
      return m_root_signature.Get();
    }
  }
}