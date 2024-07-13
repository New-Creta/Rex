#include "rex_directx/resources/dx_root_signature.h"

namespace rex
{
  namespace gfx
  {
    DxRootSignature::DxRootSignature(const wrl::ComPtr<ID3D12RootSignature>& rootSignature, rsl::vector<CD3DX12_ROOT_PARAMETER>&& rootParameters)
      : m_root_signature(rootSignature)
      , m_root_parameters(rsl::move(rootParameters))
    {
      for (s32 idx = 0; idx < m_root_parameters.size(); ++idx)
      {
        const auto& param = m_root_parameters[idx];

        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
					ShaderVisibility visibility = d3d::from_dx12(param.ShaderVisibility);
					ShaderType type = d3d::shader_visibility_to_type(visibility);

          if (param.DescriptorTable.pDescriptorRanges[0].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
          {
            m_shader_parameter_idx[type].textures_root_param_idx = idx;
          }

          else if (param.DescriptorTable.pDescriptorRanges[0].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
          {
            m_shader_parameter_idx[type].samplers_root_param_idx = idx;
          }
        }
      }
    }

    s32 DxRootSignature::param_idx_for_textures(ShaderType type)
    {
      if (m_shader_parameter_idx.contains(type))
      {
        return m_shader_parameter_idx.at(type).textures_root_param_idx;
      }

      return -1;
    }
    s32 DxRootSignature::param_idx_for_samplers(ShaderType type)
    {
      if (m_shader_parameter_idx.contains(type))
      {
        return m_shader_parameter_idx.at(type).samplers_root_param_idx;
      }

      return -1;
    }

    ID3D12RootSignature* DxRootSignature::dx_object()
    {
      return m_root_signature.Get();
    }
  }
}