#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"

#include "rex_std/numeric.h"
#include "rex_std/algorithm.h"

namespace rex
{
	namespace gfx
	{
    //const s32 g_renderpass_register_space = 0;
    //const s32 g_material_register_space = 1;

		//ShaderPipelineParameters::ShaderPipelineParameters(const ShaderPipeline& pipeline)
		//{
  //    ShaderPipelineReflection pipeline_reflection = reflect_shader_pipeline(pipeline);

  //    s32 slot_idx = 0;
		//	slot_idx = process_shader(slot_idx, pipeline_reflection.vs, ShaderVisibility::Vertex);
		//	slot_idx = process_shader(slot_idx, pipeline_reflection.ps, ShaderVisibility::Pixel);
		//}

  //  const rsl::unordered_map<ShaderType, rsl::vector<ViewTable>>& ShaderPipelineParameters::view_tables_per_shader() const
  //  {
  //    return m_view_tables_per_shader;
  //  }

  //  s32 ShaderPipelineParameters::num() const
  //  {
  //    return m_parameters.size();
  //  }
  //  ShaderParameter2* ShaderPipelineParameters::operator[](s32 idx)
  //  {
  //    return m_parameters[idx].get();
  //  }
  //  const ShaderParameter2* ShaderPipelineParameters::operator[](s32 idx) const
  //  {
  //    return m_parameters[idx].get();
  //  }

    //ShaderPipelineBindings ShaderPipelineParameters::params(ShaderParameterBinding binding) const
    //{
    //  switch (binding)
    //  {
    //  case rex::gfx::ShaderParameterBinding::RenderPass:  return m_renderpass_bindings;
    //  case rex::gfx::ShaderParameterBinding::Material:    return m_material_bindings;
    //  }

    //  return ShaderPipelineBindings();
    //}
    //const rsl::unordered_map<ShaderType, ParamBindingSlots>& ShaderPipelineParameters::shader_param_binding_slots() const
    //{
    //  return m_shader_type_to_binding_slots;
    //}

    // Process a single shader and extract all its input parameters from it
    // We split of shader input parameters to material input parameters and renderpass input parameters
		//s32 ShaderPipelineParameters::process_shader(s32 startSlot, const ShaderSignature& signature, ShaderVisibility visibility)
		//{
  //    // Split of the shader parameters by register space
  //    SplittedResources splitted_cbs = split_resources(signature.constant_buffers_resources());
  //    SplittedResources splitted_textures = split_resources(signature.textures());
  //    SplittedResources splitted_samplers = split_resources(signature.samplers());

  //    add_bindings(splitted_cbs.material_resources, m_material_bindings, ShaderResourceType::ConstantBuffer, g_material_register_space, visibility);
  //    add_bindings(splitted_textures.material_resources, m_material_bindings, ShaderResourceType::Texture, g_material_register_space, visibility);
  //    add_bindings(splitted_samplers.material_resources, m_material_bindings, ShaderResourceType::Sampler, g_material_register_space, visibility);

  //    add_bindings(splitted_cbs.renderpass_resources, m_renderpass_bindings, ShaderResourceType::ConstantBuffer, g_renderpass_register_space, visibility);
  //    add_bindings(splitted_textures.renderpass_resources, m_renderpass_bindings, ShaderResourceType::Texture, g_renderpass_register_space, visibility);
  //    add_bindings(splitted_samplers.renderpass_resources, m_renderpass_bindings, ShaderResourceType::Sampler, g_renderpass_register_space, visibility);


  //    

  //    //// Create ranges who's values are supposed to be set by a material
  //    //rsl::vector<ViewRange> material_cb_ranges = create_resource_view_ranges(splitted_cbs.material_resources, m_material_bindings, DescriptorRangeType::ConstantBufferView, g_material_register_space);
  //    //rsl::vector<ViewRange> material_texture_ranges = create_resource_view_ranges(splitted_textures.material_resources, m_material_bindings, DescriptorRangeType::ShaderResourceView, g_material_register_space);
  //    //rsl::vector<ViewRange> material_sampler_ranges = create_resource_view_ranges(splitted_samplers.material_resources, m_material_bindings, DescriptorRangeType::Sampler, g_material_register_space);

  //    //// Create ranges who's values are supposed to be set by a renderpass
  //    //rsl::vector<ViewRange> renderpass_cb_ranges = create_resource_view_ranges(splitted_cbs.renderpass_resources, m_renderpass_bindings, DescriptorRangeType::ConstantBufferView, g_renderpass_register_space);
  //    //rsl::vector<ViewRange> renderpass_texture_ranges = create_resource_view_ranges(splitted_textures.renderpass_resources, m_renderpass_bindings, DescriptorRangeType::ShaderResourceView, g_renderpass_register_space);
  //    //rsl::vector<ViewRange> renderpass_sampler_ranges = create_resource_view_ranges(splitted_samplers.renderpass_resources, m_renderpass_bindings, DescriptorRangeType::Sampler, g_renderpass_register_space);

  //    //// At this point the ranges are created and we have a cache pointing from name to the index within each range
  //    //// Now we can create the actual tables which will hold the ranges
  //    //m_shader_type_to_binding_slots[signature.shader_type()].material_cbs_slot         = add_ranges_to_table(m_material_bindings, material_cb_ranges, ShaderResourceType::ConstantBuffer, rsl::Out(startSlot));
  //    //m_shader_type_to_binding_slots[signature.shader_type()].material_textures_slot    = add_ranges_to_table(m_material_bindings, material_texture_ranges, ShaderResourceType::Texture, rsl::Out(startSlot));
  //    //m_shader_type_to_binding_slots[signature.shader_type()].material_samplers_slot    = add_ranges_to_table(m_material_bindings, material_sampler_ranges, ShaderResourceType::Sampler, rsl::Out(startSlot));
  //    //m_shader_type_to_binding_slots[signature.shader_type()].renderpass_cbs_slot       = add_ranges_to_table(m_renderpass_bindings, renderpass_cb_ranges, ShaderResourceType::ConstantBuffer, rsl::Out(startSlot));
  //    //m_shader_type_to_binding_slots[signature.shader_type()].renderpass_textures_slot  = add_ranges_to_table(m_renderpass_bindings, renderpass_texture_ranges, ShaderResourceType::Texture, rsl::Out(startSlot));
  //    //m_shader_type_to_binding_slots[signature.shader_type()].renderpass_samplers_slot  = add_ranges_to_table(m_renderpass_bindings, renderpass_sampler_ranges, ShaderResourceType::Sampler, rsl::Out(startSlot));

  //    return startSlot;
		//}

    //void ShaderPipelineParameters::add_bindings(const rsl::vector<BoundResourceReflection>& resources, ShaderPipelineBindings& targetBindings, ShaderResourceType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
    //{
    //  // The resources holds a list of each individual resource of a certain type, in a shader
    //  // We need to combine them into a set of ranges that are continious, based on register
    //  if (resources.empty())
    //  {
    //    return;
    //  }

    //  // Resource inputs will always be from the same shader, same type and register space
    //  // Chances are we have 1 continious range that can store all the resources
    //  // However, it is possible that resource registers are not continious
    //  // in which case we split the resources of in another range

    //  ViewTableBuilder view_table_builder(expectedRegisterSpace, type);
    //  s32 slot = m_parameters.size();

    //  s32 shader_resource_idx = targetBindings.resources.size(); // this stands for the view table index for now

    //  for (s32 i = 0; i < resources.size(); ++i)
    //  {
    //    const BoundResourceReflection& resource = resources[i];
    //    ViewOffset view_offset = view_table_builder.add_resource(shader_resource_idx, resource);
    //    switch (expectedRegisterSpace)
    //    {
    //    case g_renderpass_register_space: m_renderpass_parameters.add_resource(resource, slot, view_offset);
    //    case g_material_register_space:   m_material_parameters.add_resource(resource, slot, view_offset);
    //    }

    //    targetBindings.view_map.emplace(resource.name, slot, view_offset);
    //  }

    //  m_parameters.push_back(rsl::make_unique<ShaderParameter2>(view_table_builder.build(visibility)));
    //}

    //s32 ShaderPipelineParameters::add_ranges_to_table(ShaderPipelineBindings& bindings, const rsl::vector<ViewRange>& ranges, ShaderResourceType shaderResourceType, rsl::Out<s32> outSlot)
    //{
    //  if (!ranges.empty())
    //  {
    //    s32& slot = outSlot.get();
    //    ShaderResource& resource = bindings.resources.emplace_back();
    //    s32 total_num_views = rsl::accumulate(ranges.cbegin(), ranges.cend(), 0, 
    //      [](s32 val, const ViewRange& range)
    //      {
    //        return val + range.num_views;
    //      });
    //    resource.slot = slot;
    //    resource.views.resize(total_num_views);
    //    resource.type = shaderResourceType;
    //    //ViewTable& table = m_view_tables_per_shader[shaderType].emplace_back(shader_type_to_visibility(shaderType));
    //    //rsl::copy(ranges.cbegin(), ranges.cend(), rsl::back_inserter(table.ranges));
    //    return slot++; // return the old slot, but make sure to increase it for the next param
    //  }

    //  return -1;
    //}


		//ShaderPipelineParameters::SplittedResources ShaderPipelineParameters::split_resources(const rsl::vector<BoundResourceReflection> resources)
		//{
  //    SplittedResources splitted_resources{};

  //    for (const auto& resource : resources)
  //    {
  //      switch (resource.register_space)
  //      {
  //      case g_material_register_space: splitted_resources.material_resources.push_back(resource); break;
  //      case g_renderpass_register_space: splitted_resources.renderpass_resources.push_back(resource); break;
  //      default: REX_ASSERT("Invalid constant buffer register space: {}", resource.register_space); break;
  //      }
  //    }

  //    return splitted_resources;
		//}
		//rsl::vector<ViewRange> ShaderPipelineParameters::create_resource_view_ranges(const rsl::vector<BoundResourceReflection>& resources, ShaderPipelineBindings& bindings, DescriptorRangeType type, s32 expectedRegisterSpace)
		//{
  //    rsl::vector<ViewRange> ranges;
  //    s32 idx_in_view_ranges = 0;
  //    s32 num_views = 0;
  //    s32 start_register = 0;
  //    s32 expected_register = start_register;
  //    
  //    ShaderResourceType shader_resource_type;
  //    switch (type)
  //    {
  //    case rex::gfx::DescriptorRangeType::ConstantBufferView:   shader_resource_type = ShaderResourceType::ConstantBuffer;
  //    case rex::gfx::DescriptorRangeType::ShaderResourceView:   shader_resource_type = ShaderResourceType::Texture;
  //    case rex::gfx::DescriptorRangeType::Sampler:              shader_resource_type = ShaderResourceType::Sampler;
  //    }
  //    s32 total_num_views = 0;

  //    ViewTableBuilder view_table_builder(expectedRegisterSpace, type, shaderType);
  //    
  //    for (const BoundResourceReflection& resource : resources)
  //    {
  //      view_table_builder.add_resource(resource);

  //  //    // Create ranges for all constant buffers
  //  //    // A range is split up if there register is not continious
  //  //    REX_ASSERT_X(resource.register_space == expectedRegisterSpace, "Invalid register space for resource. expected: {} actual: {}", expectedRegisterSpace, resource.register_space);
  //  //    
  //  //    s32 resource_register = resource.shader_register;
  //  //    if (resource_register != expected_register)
  //  //    {
  //  //      num_views = expected_register - start_register;
  //  //      total_num_views += num_views;
  //  //      if (num_views > 0)
  //  //      {
  //  //        ranges.emplace_back(start_register, num_views, type, expectedRegisterSpace);
  //  //      }
		//		//	start_register = resource_register;
  //  //      expected_register = start_register;
  //  //    }

  //  //    bindings.view_map.emplace(resource.name, ViewParam{ idx_in_view_ranges, type, resource.shader_type });
  //  //    ++idx_in_view_ranges;
		//		//++expected_register;
  //    }
  //    //num_views = expected_register - start_register;
  //    //total_num_views += num_views;
  //    //if (num_views > 0)
  //    //{
  //    //  ranges.emplace_back(start_register, num_views, type, expectedRegisterSpace);
  //    //}

  //    return ranges;
		//}
	}
}