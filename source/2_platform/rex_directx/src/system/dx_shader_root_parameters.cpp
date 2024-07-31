#include "rex_directx/system/dx_shader_root_parameters.h"

#include "rex_renderer_core/shader_reflection/shader_signature.h"

namespace rex
{
	namespace gfx
	{
    const s32 g_material_register_space = 0;
    const s32 g_renderpass_register_space = 1;

    DxShaderPipelineParameters::DxShaderPipelineParameters(const ShaderPipelineReflection& shaderPipelineReflection)
    {
      // For every shader in the pipeline add its parameters to the shader pipeline
      // If a duplicate is found, update an existing's visibility to be visible to all shaders
      add_to_pipeline_parameters(shaderPipelineReflection.vs, ShaderVisibility::Vertex);
      add_to_pipeline_parameters(shaderPipelineReflection.ps, ShaderVisibility::Pixel);
    }

    const rsl::vector<CD3DX12_ROOT_PARAMETER>& DxShaderPipelineParameters::params() const
    {
      return m_root_parameters;
    }

    void DxShaderPipelineParameters::add_to_view_range(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, s32 startRegister, s32 lastRegister, s32 registerSpace, D3D12_DESCRIPTOR_RANGE_TYPE type)
    {
      s32 num_views_in_range = lastRegister - startRegister;

      if (num_views_in_range > 0)
      {
        ranges.emplace_back();
        ranges.back().BaseShaderRegister = startRegister;
        ranges.back().NumDescriptors = num_views_in_range;
        ranges.back().OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // We pack all our view tables together, so we can just follow from where we left of
        ranges.back().RangeType = type;
        ranges.back().RegisterSpace = registerSpace;
      }
    }

    void DxShaderPipelineParameters::add_ranges(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, const rsl::vector<BoundResourceReflection>& resources, D3D12_DESCRIPTOR_RANGE_TYPE type)
    {
      s32 start_register = 0;
      s32 expected_register = start_register;
      s32 resource_register = -1;
      s32 register_space = 0;
      s32 expected_register_space = 0;
      for (const auto& resource : resources)
      {
        resource_register = resource.shader_register;
        register_space = resource.register_space;
        if (resource_register != expected_register || register_space != expected_register_space)
        {
          // Submit new range
          add_to_view_range(ranges, start_register, expected_register, expected_register, type);
          start_register = resource_register;
          register_space = expected_register_space;
        }

        switch (register_space)
        {
        case g_material_register_space: m_material_parameters.emplace_back(resource.name, -1, resource.shader_register, resource.resource_type); break;
        case g_renderpass_register_space: m_renderpass_parameters.emplace_back(resource.name, -1, resource.shader_register, resource.resource_type); break;
        default: REX_ASSERT("Invalid shader registe space: {}", register_space);
        }

        ++expected_register;
      }
      add_to_view_range(ranges, start_register, expected_register, expected_register_space, type);
    }

    void DxShaderPipelineParameters::add_to_pipeline_parameters(const ShaderSignature& signature, ShaderVisibility shaderVis)
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

      // 2. Sort out all the views
      for (const auto& cb : signature.constant_buffers())
      {
        m_root_parameters.emplace_back().InitAsConstantBufferView(cb.shader_register, cb.regiser_space, visibility);
        switch (cb.regiser_space)
        {
        case g_material_register_space:       m_material_parameters.emplace_back(cb.name, cb.size, cb.shader_register, ShaderParameterType::ConstantBuffer);    break;
        case g_renderpass_register_space:     m_renderpass_parameters.emplace_back(cb.name, cb.size, cb.shader_register, ShaderParameterType::ConstantBuffer);  break;
        default: REX_ASSERT("Unknown shader register space: {}", cb.regiser_space); break;
        }

      }

      // 3. Sort out all the view tables
      add_ranges(m_texture_ranges, signature.textures(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
      add_ranges(m_sampler_ranges, signature.samplers(), D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);

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

	}
}