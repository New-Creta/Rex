#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/materials/material_system.h"

#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogMaterial);

    Material::Material(ShaderPipeline&& shaderPipeline, const MaterialConstructSettings& matConstructSettings)
      : m_shader_pipeline(rsl::move(shaderPipeline))
      , m_primitive_topology(PrimitiveTopology::TriangleList)
      , m_root_signature()
      , m_blend_factor()
      , m_param_name_to_view_idx()
    {
      // We calculate the shader reflection so we know which parameters this material needs      
      init();

      auto shader_pipeline_reflection = reflect_shader_pipeline(m_shader_pipeline);
      m_input_layout_desc = create_input_layout_desc_from_reflection(shader_pipeline_reflection.vs.input_params());

      m_root_signature = rhi::create_root_signature(m_shader_pipeline);
      m_raster_state = matConstructSettings.raster_state;
      m_blend = matConstructSettings.blend;
      m_depth_stencil = matConstructSettings.depth_stencil;
    }

    void Material::init()
    {
      ShaderPipelineParameters parameters(m_shader_pipeline);
      m_param_name_to_view_idx = parameters.material_params();

      for (const auto& [name, param] : m_param_name_to_view_idx)
      {
        switch (param.type)
        {
        case DescriptorRangeType::ConstantBufferView:   m_constant_buffers.emplace_back(param.shader_type); break;
        case DescriptorRangeType::ShaderResourceView:   m_textures.emplace_back(param.shader_type); break;
        case DescriptorRangeType::Sampler:              m_samplers.emplace_back(param.shader_type); break;
        }

        m_param_binding_indices[param.shader_type] = parameters.shader_param_binding_slots(param.shader_type);
      }
    }

    void Material::set(rsl::string_view name, ConstantBuffer* constantBuffer)
    {
      s32 idx = index_of_param(name);
      m_constant_buffers[idx].set_resource(constantBuffer);
    }
    void Material::set(rsl::string_view name, Texture2D* texture)
    {
      s32 idx = index_of_param(name);
      m_textures[idx].set_resource(texture);
    }
    void Material::set(rsl::string_view name, Sampler2D* sampler)
    {
      s32 idx = index_of_param(name);
      m_samplers[idx].set_resource(sampler);
    }

    rsl::unordered_map<ShaderType, ShaderResources> Material::shader_resources()
    {
      rsl::unordered_map<ShaderType, ShaderResources> shader_resources{};

      for (auto& cb : m_constant_buffers)
      {
        shader_resources[cb.type()].constant_buffers.push_back(static_cast<ConstantBuffer*>(cb.resource()));
      }
      for (auto& texture : m_textures)
      {
        shader_resources[texture.type()].textures.push_back(static_cast<Texture2D*>(texture.resource()));
      }
      for (auto& sampler : m_samplers)
      {
        shader_resources[sampler.type()].samplers.push_back(static_cast<Sampler2D*>(sampler.resource()));
      }

      for (auto& [shader_type, resources] : shader_resources)
      {
        resources.cb_slot = m_param_binding_indices.at(shader_type).material_cbs;
        resources.srv_slot = m_param_binding_indices.at(shader_type).material_textures;
        resources.sampler_slot = m_param_binding_indices.at(shader_type).material_samplers;
      }

      return shader_resources;
    }


    void Material::set_blend_factor(const BlendFactor& blendFactor)
    {
      m_blend_factor = blendFactor;
    }

    PrimitiveTopology Material::primitive_topology() const
    {
      return m_primitive_topology;
    }
    RootSignature* Material::root_signature()
    {
      return m_root_signature.get();
    }
    BlendFactor Material::blend_factor()
    {
      return m_blend_factor;
    }

    s32 Material::index_of_param(rsl::string_view name)
    {
      return m_param_name_to_view_idx.at(name).idx;
    }

    //ShaderResourceView Material::resources_for_shader(ShaderType type)
    //{
    //  ShaderResourceView resources{};

    //  for (auto& texture : m_textures)
    //  {
    //    if (texture.value.shader_type() != type)
    //    {
    //      continue;
    //    }

    //    resources.textures.push_back(&texture.value);
    //  }
    //  for (auto& sampler : m_samplers)
    //  {
    //    if (sampler.value.shader_type() != type)
    //    {
    //      continue;
    //    }

    //    // It's possible a static sampler was provided and its therefore not a member of the material
    //    if (sampler.value.sampler())
    //    {
    //      resources.samplers.push_back(&sampler.value);
    //    }
    //  }

    //  resources.textures_root_param_idx = m_root_signature->param_idx_for_textures(type);
    //  resources.samplers_root_param_idx = m_root_signature->param_idx_for_samplers(type);

    //  return resources;
    //}

    void Material::fill_pso_desc(PipelineStateDesc& desc)
    {
      desc.primitive_topology = to_topology_type(m_primitive_topology);
      desc.shader_pipeline = m_shader_pipeline;
      desc.raster_state = m_raster_state;
      desc.blend_state = m_blend;
      desc.depth_stencil_state = m_depth_stencil;
    }

    void Material::validate_input_layout(InputLayout* inputLayout)
    {
      REX_UNUSED_PARAM(inputLayout);

      REX_ASSERT_X(inputLayout->validate_desc(m_input_layout_desc), "Input layout is not compatible with material");
    }

    void Material::bind_resources(RenderContext* ctx)
    {
      // Any resource that's not set by the render pass should be set by the material
      //for (const auto& parameter : m_shader_parameters)
      //{
      //  // pure virtual function which will bind the parameter to the ctx based on the type of the parameter
      //  parameter->bind(ctx);
      //}



    }

  }
}