#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"

namespace rex
{
	namespace gfx
	{
    const s32 g_renderpass_register_space = 0;
    const s32 g_material_register_space = 1;

		ShaderPipelineParameters::ShaderPipelineParameters(const ShaderPipeline& pipeline)
		{
      ShaderPipelineReflection pipeline_reflection = reflect_shader_pipeline(pipeline);

      s32 slot_idx = 0;
			slot_idx = process_shader(slot_idx, pipeline_reflection.vs, ShaderVisibility::Vertex);
			slot_idx = process_shader(slot_idx, pipeline_reflection.ps, ShaderVisibility::Pixel);
		}

    //const rsl::vector<ViewTable>& ShaderPipelineParameters::material_view_tables() const
    //{
    //  return m_material_view_tables;
    //}
    //const rsl::vector<ViewTable>& ShaderPipelineParameters::renderpass_view_tables() const
    //{
    //  return m_renderpass_view_tables;
    //}
    const rsl::unordered_map<ShaderType, rsl::vector<ViewTable>>& ShaderPipelineParameters::view_tables_per_shader() const
    {
      return m_view_tables_per_shader;
    }

    rsl::unordered_map<rsl::string, ViewParam> ShaderPipelineParameters::material_params()
    {
      return m_material_param_name_to_idx;
    }
    rsl::unordered_map<rsl::string, ViewParam> ShaderPipelineParameters::renderpass_params()
    {
      return m_renderpass_param_name_to_idx;
    }
    ParamBindingIndices ShaderPipelineParameters::shader_param_binding_slots(ShaderType type) const
    {
      if (m_shader_type_to_binding_indices.contains(type))
      {
        return m_shader_type_to_binding_indices.at(type);
      }

      return ParamBindingIndices{};
    }


		s32 ShaderPipelineParameters::process_shader(s32 slotIdx, const ShaderSignature& signature, ShaderVisibility visibility)
		{
      // Split of the shader parameters by register space
      SplittedResources splitted_cbs = split_resources(signature.constant_buffers_resources());
      SplittedResources splitted_textures = split_resources(signature.textures());
      SplittedResources splitted_samplers = split_resources(signature.samplers());

      // Create ranges who's values are supposed to be set by a material
      rsl::vector<ViewRange> material_cb_ranges = create_resource_view_ranges(splitted_cbs.material_resources, m_material_param_name_to_idx, DescriptorRangeType::ConstantBufferView, g_material_register_space);
      rsl::vector<ViewRange> material_texture_ranges = create_resource_view_ranges(splitted_textures.material_resources, m_material_param_name_to_idx, DescriptorRangeType::ShaderResourceView, g_material_register_space);
      rsl::vector<ViewRange> material_sampler_ranges = create_resource_view_ranges(splitted_samplers.material_resources, m_material_param_name_to_idx, DescriptorRangeType::Sampler, g_material_register_space);

      // Create ranges who's values are supposed to be set by a renderpass
      rsl::vector<ViewRange> renderpass_cb_ranges = create_resource_view_ranges(splitted_cbs.renderpass_resources, m_renderpass_param_name_to_idx, DescriptorRangeType::ConstantBufferView, g_renderpass_register_space);
      rsl::vector<ViewRange> renderpass_texture_ranges = create_resource_view_ranges(splitted_textures.renderpass_resources, m_renderpass_param_name_to_idx, DescriptorRangeType::ShaderResourceView, g_renderpass_register_space);
      rsl::vector<ViewRange> renderpass_sampler_ranges = create_resource_view_ranges(splitted_samplers.renderpass_resources, m_renderpass_param_name_to_idx, DescriptorRangeType::Sampler, g_renderpass_register_space);

      // At this point the ranges are created and we have a cache pointing from name to the index within each range
      // Now we can create the actual tables which will hold the ranges
      if (!material_cb_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].material_cbs = slotIdx++;
        ViewTable& material_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(material_cb_ranges.cbegin(), material_cb_ranges.cend(), rsl::back_inserter(material_table.ranges));
      }
      if (!material_texture_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].material_textures = slotIdx++;
        ViewTable& material_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(material_texture_ranges.cbegin(), material_texture_ranges.cend(), rsl::back_inserter(material_table.ranges));
      }
      if (!material_sampler_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].material_samplers = slotIdx++;
        ViewTable& material_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(material_sampler_ranges.cbegin(), material_sampler_ranges.cend(), rsl::back_inserter(material_table.ranges));
      }

      if (!renderpass_cb_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].renderpass_cbs = slotIdx++;
        ViewTable& renderpass_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(renderpass_cb_ranges.cbegin(), renderpass_cb_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
      }
      if (!renderpass_texture_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].renderpass_textures = slotIdx++;
        ViewTable& renderpass_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(renderpass_texture_ranges.cbegin(), renderpass_texture_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
      }
      if (!renderpass_sampler_ranges.empty())
      {
        m_shader_type_to_binding_indices[signature.shader_type()].renderpass_samplers = slotIdx++;
        ViewTable& renderpass_table = m_view_tables_per_shader[signature.shader_type()].emplace_back(visibility);
        rsl::copy(renderpass_sampler_ranges.cbegin(), renderpass_sampler_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
      }

      return slotIdx;
		}

		ShaderPipelineParameters::SplittedResources ShaderPipelineParameters::split_resources(const rsl::vector<BoundResourceReflection> resources)
		{
      SplittedResources splitted_resources{};

      for (const auto& resource : resources)
      {
        switch (resource.register_space)
        {
        case g_material_register_space: splitted_resources.material_resources.push_back(resource); break;
        case g_renderpass_register_space: splitted_resources.renderpass_resources.push_back(resource); break;
        default: REX_ASSERT("Invalid constant buffer register space: {}", resource.register_space); break;
        }
      }

      return splitted_resources;
		}
		rsl::vector<ViewRange> ShaderPipelineParameters::create_resource_view_ranges(const rsl::vector<BoundResourceReflection>& resources, rsl::unordered_map<rsl::string, ViewParam>& nameToIdxMap, DescriptorRangeType type, s32 expectedRegisterSpace)
		{
      rsl::vector<ViewRange> ranges;
      s32 idx_in_view_ranges = 0;
      s32 num_views = 0;
      s32 start_register = 0;
      s32 expected_register = start_register;
      
      for (const BoundResourceReflection& resource : resources)
      {
        // Create ranges for all constant buffers
        // A range is split up if there register is not continious
        REX_ASSERT_X(resource.register_space == expectedRegisterSpace, "Invalid register space for resource. expected: {} actual: {}", expectedRegisterSpace, resource.register_space);
        
        s32 resource_register = resource.shader_register;
        if (resource_register != expected_register)
        {
          num_views = expected_register - start_register;
          if (num_views > 0)
          {
            ranges.emplace_back(start_register, num_views, type, expectedRegisterSpace);
          }
					start_register = resource_register;
          expected_register = start_register;
        }

        nameToIdxMap.emplace(resource.name, ViewParam{ idx_in_view_ranges, type, resource.shader_type });
        ++idx_in_view_ranges;
				++expected_register;
      }
      num_views = expected_register - start_register;
      if (num_views > 0)
      {
        ranges.emplace_back(start_register, num_views, type, expectedRegisterSpace);
      }

      return ranges;
		}
	}
}