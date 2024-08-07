//#include "rex_directx/system/dx_shader_root_parameters.h"
//
//#include "rex_renderer_core/shader_reflection/shader_signature.h"
//
//
//
//namespace rex
//{
//	namespace gfx
//	{
//    const s32 g_material_register_space = 0;
//    const s32 g_renderpass_register_space = 1;
//
//    enum class RootParameterType
//    {
//      View,
//      ViewTable
//    };
//
//    struct ShaderParameter
//    {
//      ShaderParameter(s32 slot, RootParameterType type, ShaderVisibility visibility)
//        : slot(slot)
//        , type(type)
//        , visibility(visibility)
//      {}
//
//      virtual void set(Resource* newResource)
//      {
//        REX_ASSERT("Invalid set call on shader parameter, not sure what to do here");
//      }
//
//      s32 slot;
//      RootParameterType type;
//      ShaderVisibility visibility;
//    };
//
//    // A parameter that holds a single view
//    struct ViewParameter : ShaderParameter
//    {
//      ViewParameter(s32 slot, rsl::string_view name, s32 shaderRegister, s32 registerSpace, s32 size, ShaderVisibility visibility)
//        : ShaderParameter(slot, RootParameterType::View, visibility)
//        , name(name)
//        , shader_register(shaderRegister)
//        , register_space(registerSpace)
//        , size(size)
//      {}
//
//      void set(Resource* newResource) override
//      {
//        resource = newResource;
//      }
//
//      rsl::string name;
//      s32 register_space;
//      s32 shader_register;
//      s32 size;
//      Resource* resource;
//    };
//
//    // A paremter that holds a list of a list of views
//    //
//    // ViewTable
//    // --> View List A
//    // ----> View A
//    // ----> View B
//    // --> View List B
//    // ----> View C
//    // ----> View D
//    struct ViewParameterInTable : ShaderParameter
//    {
//      ViewParameterInTable(s32 slot, rsl::string_view name, s32 shaderRegister, s32 registerSpace, s32 size, ShaderVisibility visibility)
//        : ShaderParameter(slot, RootParameterType::View, visibility)
//        , name(name)
//        , shader_register(shaderRegister)
//        , register_space(registerSpace)
//        , size(size)
//      {}
//
//      void set(Resource* newResource) override
//      {
//        resource = newResource;
//      }
//
//      rsl::string name;
//      s32 register_space;
//      s32 shader_register;
//      s32 size;
//      Resource* resource;
//    };
//    struct ViewTableParameter;
//    // All views in a range should be in the same register space,
//    // But a view table can hold ranges that are in different register spaces
//    // However, for optimization purposes, we split the descriptor tables based on register space
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//  //  DxShaderPipelineParameters::DxShaderPipelineParameters(const ShaderPipelineReflection& shaderPipelineReflection)
//  //  {
//  //    process_shader(shaderPipelineReflection.vs, ShaderVisibility::Vertex);
//  //    process_shader(shaderPipelineReflection.ps, ShaderVisibility::Pixel);
//
//
//
//
//
//
//
//
//
//
//  //    // First build up the parameters for the root signature
//  //    // The root signature doesn't care if a parameter belongs to a material or render pass
//  //    add_to_pipeline_parameters(shaderPipelineReflection.vs, ShaderVisibility::Vertex);
//  //    add_to_pipeline_parameters(shaderPipelineReflection.ps, ShaderVisibility::Pixel);
//  //    
//  //    // Next split up the parameters based on their register space
//  //    // As this determines if a parameter should be set by a render pass or a material
//  //    split_parameters_based_on_register_space(shaderPipelineReflection);
//
//  //    // The end result of the constructor should have a list of root parameters (atm a view table for materials and for renderpasses)
//  //    // as well as have a hash table mapping a resource name to an index within the view table
//  //  }
//
//  //  DxShaderPipelineParameters::SplittedConstantBuffers DxShaderPipelineParameters::split_constant_buffers(const rsl::vector<CBufferReflDesc> constantBuffers)
//  //  {
//  //    SplittedConstantBuffers splitted_cbs{};
//
//  //    for (const auto& cb : constantBuffers)
//  //    {
//  //      switch (cb.register_space)
//  //      {
//  //      case g_material_register_space: splitted_cbs.material_cbs.push_back(cb); break;
//  //      case g_renderpass_register_space: splitted_cbs.renderpass_cbs.push_back(cb); break;
//  //      default: REX_ASSERT("Invalid constant buffer register space: {}", cb.register_space); break;
//  //      }
//  //    }
//
//  //    return splitted_cbs;
//  //  }
//  //  DxShaderPipelineParameters::SplittedResources DxShaderPipelineParameters::split_resources(const rsl::vector<BoundResourceReflection> resources)
//  //  {
//  //    SplittedResources splitted_resources{};
//
//  //    for (const auto& resource : resources)
//  //    {
//  //      switch (resource.register_space)
//  //      {
//  //      case g_material_register_space: splitted_resources.material_resources.push_back(resource); break;
//  //      case g_renderpass_register_space: splitted_resources.renderpass_resources.push_back(resource); break;
//  //      default: REX_ASSERT("Invalid constant buffer register space: {}", resource.register_space); break;
//  //      }
//  //    }
//
//  //    return splitted_resources;
//  //  }
//
//  //  rsl::vector<ViewRange> DxShaderPipelineParameters::create_cb_view_ranges(const rsl::vector<CBufferReflDesc>& cbs, rsl::unordered_map<rsl::string, s32>& nameToIdxMap)
//  //  {
//  //    rsl::vector<ViewRange> ranges;
//  //    s32 idx_in_view_ranges = 0;
//  //    s32 num_views = 0;
//  //    s32 start_register = 0;
//  //    s32 expected_register = start_register;
//  //    for (const CBufferReflDesc& cb : cbs)
//  //    {
//  //      // Create ranges for all constant buffers
//  //      // A range is split up if there register is not continious
//  //      s32 resource_register = cb.shader_register;
//  //      if (resource_register != expected_register)
//  //      {
//  //        num_views = expected_register - start_register;
//  //        if (num_views > 0)
//  //        {
//  //          ranges.emplace_back(start_register, num_views, DescriptorRangeType::ConstantBufferView);
//  //        }
//  //      }
//
//  //      nameToIdxMap.emplace(cb.name, idx_in_view_ranges);
//  //      ++idx_in_view_ranges;
//  //    }
//  //    num_views = expected_register - start_register;
//  //    if (num_views > 0)
//  //    {
//  //      ranges.emplace_back(start_register, num_views, DescriptorRangeType::ConstantBufferView);
//  //    }
//
//  //    return ranges;
//  //  }
//  //  rsl::vector<ViewRange> DxShaderPipelineParameters::create_resource_view_ranges(const rsl::vector<BoundResourceReflection>& resources, rsl::unordered_map<rsl::string, s32>& nameToIdxMap, DescriptorRangeType type)
//  //  {
//  //    rsl::vector<ViewRange> ranges;
//  //    s32 idx_in_view_ranges = 0;
//  //    s32 num_views = 0;
//  //    s32 start_register = 0;
//  //    s32 expected_register = start_register;
//  //    for (const BoundResourceReflection& resource : resources)
//  //    {
//  //      // Create ranges for all constant buffers
//  //      // A range is split up if there register is not continious
//  //      s32 resource_register = resource.shader_register;
//  //      if (resource_register != expected_register)
//  //      {
//  //        num_views = expected_register - start_register;
//  //        if (num_views > 0)
//  //        {
//  //          ranges.emplace_back(start_register, num_views, type);
//  //        }
//  //      }
//
//  //      nameToIdxMap.emplace(resource.name, idx_in_view_ranges);
//  //      ++idx_in_view_ranges;
//  //    }
//  //    num_views = expected_register - start_register;
//  //    if (num_views > 0)
//  //    {
//  //      ranges.emplace_back(start_register, num_views, type);
//  //    }
//
//  //    return ranges;
//  //  }
//
//  //  void DxShaderPipelineParameters::process_shader(const ShaderSignature& signature, ShaderVisibility visibility)
//  //  {
//  //    // Split of the shader parameters by register space
//  //    SplittedConstantBuffers splitted_cbs = split_constant_buffers(signature.constant_buffers());
//  //    SplittedResources splitted_textures = split_resources(signature.textures());
//  //    SplittedResources splitted_samplers = split_resources(signature.samplers());
//
//  //    // Create ranges who's values are supposed to be set by a material
//  //    rsl::vector<ViewRange> material_cb_ranges = create_cb_view_ranges(splitted_cbs.material_cbs, m_material_param_name_to_offset_from_start);
//  //    rsl::vector<ViewRange> material_texture_ranges = create_resource_view_ranges(splitted_textures.material_resources, m_material_param_name_to_offset_from_start);
//  //    rsl::vector<ViewRange> material_sampler_ranges = create_resource_view_ranges(splitted_samplers.material_resources, m_material_param_name_to_offset_from_start);
//
//  //    // Create ranges who's values are supposed to be set by a renderpass
//  //    rsl::vector<ViewRange> renderpass_cb_ranges = create_cb_view_ranges(splitted_cbs.renderpass_cbs, m_renderpass_param_name_to_offset_from_start);
//  //    rsl::vector<ViewRange> renderpass_texture_ranges = create_resource_view_ranges(splitted_textures.renderpass_resources, m_renderpass_param_name_to_offset_from_start);
//  //    rsl::vector<ViewRange> renderpass_sampler_ranges = create_resource_view_ranges(splitted_samplers.renderpass_resources, m_renderpass_param_name_to_offset_from_start);
//
//  //    // At this point the ranges are created and we have a cache pointing from name to the index within each range
//  //    // Now we can create the actual tables which will hold the ranges
//  //    ViewTable& material_table = m_view_tables.emplace_back(visibility);
//  //    rsl::copy(material_cb_ranges.cbegin(), material_cb_ranges.cend(), rsl::back_inserter(material_table.ranges));
//  //    rsl::copy(material_texture_ranges.cbegin(), material_texture_ranges.cend(), rsl::back_inserter(material_table.ranges));
//  //    rsl::copy(material_sampler_ranges.cbegin(), material_sampler_ranges.cend(), rsl::back_inserter(material_table.ranges));
//
//  //    ViewTable& renderpass_table = m_view_tables.emplace_back(visibility);
//  //    rsl::copy(renderpass_cb_ranges.cbegin(), renderpass_cb_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
//  //    rsl::copy(renderpass_texture_ranges.cbegin(), renderpass_texture_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
//  //    rsl::copy(renderpass_sampler_ranges.cbegin(), renderpass_sampler_ranges.cend(), rsl::back_inserter(renderpass_table.ranges));
//  //  }
//
//
//  //  void DxShaderPipelineParameters::split_off_view(ShaderParameter* param)
//  //  {
//  //    ViewParameter* view = static_cast<ViewParameter*>(param);
//
//		//	switch (view->register_space)
//		//	{
//		//	case g_material_register_space: m_material_parameters.push_back(view); break;
//		//	case g_renderpass_register_space: m_renderpass_parameters.push_back(view); break;
//		//	default: break;
//		//	}
//		//}
//  //  void DxShaderPipelineParameters::split_off_table(ShaderParameter* param)
//  //  {
//  //    ViewTableParameter* table = static_cast<ViewTableParameter*>(param);
//
//  //    for (ViewRange& range : table->ranges)
//  //    {
//  //      for (ViewParameterInTable& view : range.views)
//  //      {
//  //        switch (view.register_space)
//  //        {
//  //        case g_material_register_space: m_material_parameters.push_back(&view);   break;
//  //        case g_renderpass_register_space: m_renderpass_parameters.push_back(&view); break;
//  //        default:
//  //          break;
//  //        }
//  //      }
//  //    }
//  //  }
//
//  //  
//
//  //  const rsl::vector<CD3DX12_ROOT_PARAMETER>& DxShaderPipelineParameters::params() const
//  //  {
//  //    return m_root_parameters;
//  //  }
//
//  //  void DxShaderPipelineParameters::add_to_view_range(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, s32 startRegister, s32 lastRegister, s32 registerSpace, D3D12_DESCRIPTOR_RANGE_TYPE type)
//  //  {
//  //    REX_ASSERT_X(m_parameters.back()->type == RootParameterType::ViewTable, "Expected last parameter to be the view table");
//  //    ViewTableParameter* view_table = static_cast<ViewTableParameter*>(m_parameters.back().get());
//  //    s32 num_views_in_range = lastRegister - startRegister;
//
//  //    if (num_views_in_range > 0)
//  //    {
//  //      ranges.emplace_back();
//  //      ranges.back().BaseShaderRegister = startRegister;
//  //      ranges.back().NumDescriptors = num_views_in_range;
//  //      ranges.back().OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // We pack all our view tables together, so we can just follow from where we left of
//  //      ranges.back().RangeType = type;
//  //      ranges.back().RegisterSpace = registerSpace;
//
//  //      auto& range = view_table->table.emplace_back(view_table);
//  //      s32 shader_register = startRegister;
//  //      for (s32 i = 0; i < num_views_in_range; ++i, ++shader_register)
//  //      {
//  //        auto& view = range.views.emplace_back();
//  //        view.shader_register = shader_register;
//  //        view.register_space = registerSpace;
//  //        view.
//  //      }
//  //    }
//  //  }
//
//  //  // For a given set of resources, create ranges that point to those resources that are in continious registers
//  //  void DxShaderPipelineParameters::add_ranges(rsl::vector<D3D12_DESCRIPTOR_RANGE>& ranges, const rsl::vector<BoundResourceReflection>& resources, D3D12_DESCRIPTOR_RANGE_TYPE type)
//  //  {
//  //    s32 start_register = 0;
//  //    s32 expected_register = start_register;
//  //    s32 resource_register = -1;
//  //    s32 register_space = 0;
//  //    s32 expected_register_space = 0;
//  //    for (const auto& resource : resources)
//  //    {
//  //      s32 resource_register = resource.shader_register;
//  //      register_space = resource.register_space;
//  //      if (resource_register != expected_register || register_space != expected_register_space)
//  //      {
//  //        // Submit new range
//  //        add_to_view_range(ranges, start_register, expected_register, expected_register, type);
//  //        start_register = resource_register;
//  //        register_space = expected_register_space;
//  //      }
//
//  //      ++expected_register;
//  //    }
//  //    add_to_view_range(ranges, start_register, expected_register, expected_register_space, type);
//  //  }
//
//  //  //void DxShaderPipelineParameters::add_ranges(ViewTableParameter* table, const rsl::vector<CBufferReflDesc>& cbuffers, D3D12_DESCRIPTOR_RANGE_TYPE type)
//  //  //{
//  //  //  if (cbuffers.empty())
//  //  //  {
//  //  //    return;
//  //  //  }
//
//  //  //  ViewRange& range = table->ranges.emplace_back(table);
//  //  //  s32 slot = -1; // As the table gets bound and not the view itself, the slot is -1
//  //  //  s32 size = -1; // As the size is likely irrelevant, we set it as -1
//  //  //  for (const CBufferReflDesc& resource : cbuffers)
//  //  //  {
//  //  //    range.views.emplace_back(slot, resource.name, resource.shader_register, resource.register_space, size, table->visibility);
//  //  //  }
//
//  //  //}
//  //  //void DxShaderPipelineParameters::add_ranges(ViewTableParameter* table, const rsl::vector<BoundResourceReflection>& resources, D3D12_DESCRIPTOR_RANGE_TYPE type)
//  //  //{
//  //  //  if (resources.empty())
//  //  //  {
//  //  //    return;
//  //  //  }
//
//  //  //  ViewRange& range = table->ranges.emplace_back(table);
//  //  //  s32 slot = -1; // As the table gets bound and not the view itself, the slot is -1
//  //  //  s32 size = -1; // As the size is likely irrelevant, we set it as -1
//  //  //  for (const BoundResourceReflection& resource : resources)
//  //  //  {
//  //  //    range.views.emplace_back(slot, resource.name, resource.shader_register, resource.register_space, size, table->visibility);
//  //  //  }
//  //  //}
//
//  //  void DxShaderPipelineParameters::add_to_pipeline_parameters(const ShaderSignature& signature, ShaderVisibility shaderVis)
//  //  {
//  //    // 1. Constants (not able to retrieve this from reflection yet)
//  //    // 2. View 
//  //    // - constant buffer, 
//  //    // - SRV/UAV pointing to structured buffers or byte address buffers, 
//  //    // - raytracing acceleration structures
//  //    // 3. View Table
//  //    // - textures
//  //    // - samplers
//
//  //    // 2. Sort out all the views
//  //    //D3D12_SHADER_VISIBILITY visibility = d3d::to_dx12(shaderVis);
//  //    //for (const auto& cb : signature.constant_buffers())
//  //    //{
//  //    //  m_root_parameters.emplace_back().InitAsConstantBufferView(cb.shader_register, cb.regiser_space, visibility);
//  //    //  auto& view_param = m_parameters.emplace_back(rsl::make_unique<ViewParameter>(m_parameters.size(), cb.name, cb.shader_register, cb.regiser_space, cb.size, shaderVis));
//  //    //}
//
//  //    //// 3. Sort out all the view tables
//  //    //m_root_parameters.emplace_back(rsl::make_unique<ViewTableParameter>(m_parameters.size(), shaderVis));
//  //    //add_ranges(static_cast<ViewTableParameter*>(m_parameters.back().get()), signature.textures(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
//  //    //add_ranges(static_cast<ViewTableParameter*>(m_parameters.back().get()), signature.samplers(), D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
//
//  //    //add_ranges(m_textures_and_samplers, signature.textures(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
//  //    //add_ranges(m_textures_and_samplers, signature.samplers(), D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
//
//  //    //if (m_textures_and_samplers.size() > 0)
//  //    //{
//  //    //  m_root_parameters.emplace_back().InitAsDescriptorTable(m_textures_and_samplers.size(), m_textures_and_samplers.data(), visibility);
//  //    //}
//  //  }
//
//	}
//}