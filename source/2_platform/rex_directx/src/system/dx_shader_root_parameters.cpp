#include "rex_directx/system/dx_shader_root_parameters.h"

#include "rex_renderer_core/shader_reflection/shader_signature.h"

namespace rex
{
	namespace gfx
	{
		DxShaderRootParameters::DxShaderRootParameters(const ShaderSignature* signature, ShaderVisibility shaderVis)
		{
      D3D12_SHADER_VISIBILITY visibility = d3d::to_dx12(shaderVis);

      // 1. Constants (not able to retrieve this from reflection yet)
      // 2. View 
      // - constant buffer, 
      // - SRV/UAV pointing to structured buffers or byte address buffers, 
      // - raytracing acceleration structures
      // 3. View Table
      // - textures
      // - samplers

      UINT register_space = 0;

      // 2. Sort out all the views
      for (const auto& cb : signature->constant_buffers())
      {
        m_root_parameters.emplace_back().InitAsConstantBufferView(cb.shader_register, register_space, visibility);
      }

      // 3. Sort out all the view tables
      init_ranges(m_texture_ranges, signature->textures());
      init_ranges(m_sampler_ranges, signature->samplers());

      // Submit the ranges to the descriptor table
      if (m_texture_ranges.size() > 0)
      {
        m_root_parameters.emplace_back().InitAsDescriptorTable(m_texture_ranges.size(), m_texture_ranges.data(), visibility);
      }
      if (m_sampler_ranges.size() > 0)
      {
        m_root_parameters.emplace_back().InitAsDescriptorTable(m_sampler_ranges.size(), m_sampler_ranges.data(), visibility);
      }
		}

    const rsl::vector<CD3DX12_ROOT_PARAMETER>& DxShaderRootParameters::params() const
    {
      return m_root_parameters;
    }
    s32 DxShaderRootParameters::count() const
    {
      return m_root_parameters.size();
    }

    void DxShaderRootParameters::add_to_view_range(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, s32 startRegister, s32 lastRegister, D3D12_DESCRIPTOR_RANGE_TYPE type)
    {
      s32 num_views_in_range = lastRegister - startRegister;

      if (num_views_in_range > 0)
      {
        ranges.emplace_back();
        ranges.back().BaseShaderRegister = startRegister;
        ranges.back().NumDescriptors = num_views_in_range;
        ranges.back().OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // We pack all our view tables together, so we can just follow from where we left of
        ranges.back().RangeType = type;
        ranges.back().RegisterSpace = 0;
      }
    }

    void DxShaderRootParameters::init_ranges(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, const rsl::vector<BoundResourceReflection>& resources)
    {
      s32 start_register = 0;
      s32 current_register = start_register;
      s32 resource_register = -1;

      for (const auto& resource : resources)
      {
        resource_register = resource.shader_register;
        if (resource_register != current_register)
        {
          // Submit new range
          add_to_view_range(m_texture_ranges, start_register, current_register, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
          start_register = resource_register;
        }
        ++current_register;
      }
      add_to_view_range(m_texture_ranges, start_register, current_register, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
    }

	}
}