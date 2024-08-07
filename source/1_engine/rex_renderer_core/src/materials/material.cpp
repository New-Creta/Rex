#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/materials/material_system.h"

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
    {
      // We calculate the shader reflection so we know which parameters this material needs
      ShaderPipelineReflection shader_pipeline_reflection = reflect_shader_pipeline(m_shader_pipeline);

      init_parameters_from_shader_signature(ShaderType::Vertex, shader_pipeline_reflection.vs);
      init_parameters_from_shader_signature(ShaderType::Pixel, shader_pipeline_reflection.ps);

      m_input_layout_desc = create_input_layout_desc_from_reflection(shader_pipeline_reflection.vs.input_params());

      m_root_signature = rhi::create_root_signature(shader_pipeline_reflection);
      m_raster_state = matConstructSettings.raster_state;
      m_blend = matConstructSettings.blend;
      m_depth_stencil = matConstructSettings.depth_stencil;
    }

    void Material::init_parameters_from_shader_signature(ShaderType type, const ShaderSignature& signature)
    {
      for (const BoundResourceReflection& texture : signature.textures())
      {
        REX_ASSERT_X(!m_textures.contains(texture.name), "{} is already found in the material parameters, you like have a parameter in your shader with the same name, this is not supported. Shader: {}", rsl::enum_refl::enum_name(type));
        TextureMaterialParameter tex_param(texture.name, type, texture.shader_register);
        m_textures.emplace(texture.name, rsl::move(tex_param));
      }

      for (const BoundResourceReflection& sampler : signature.samplers())
      {
        REX_ASSERT_X(!m_samplers.contains(sampler.name), "{} is already found in the material parameters, you like have a parameter in your shader with the same name, this is not supported. Shader: {}", rsl::enum_refl::enum_name(type));
        SamplerMaterialParameter sampler_param(sampler.name, type, sampler.shader_register);
        m_samplers.emplace(sampler.name, rsl::move(sampler_param));
      }
    }

    void Material::set_texture(rsl::string_view name, Texture2D* texture)
    {
      m_textures.at(name).set(texture);
    }
    void Material::set_sampler(rsl::string_view name, Sampler2D* sampler)
    {
      m_samplers.at(name).set(sampler);
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

    AllShaderResources Material::resources_for_shader(ShaderType type)
    {
      AllShaderResources resources{};

      for (auto& texture : m_textures)
      {
        if (texture.value.shader_type() != type)
        {
          continue;
        }

        resources.textures.push_back(&texture.value);
      }
      for (auto& sampler : m_samplers)
      {
        if (sampler.value.shader_type() != type)
        {
          continue;
        }

        // It's possible a static sampler was provided and its therefore not a member of the material
        if (sampler.value.sampler())
        {
          resources.samplers.push_back(&sampler.value);
        }
      }

      resources.textures_root_param_idx = m_root_signature->param_idx_for_textures(type);
      resources.samplers_root_param_idx = m_root_signature->param_idx_for_samplers(type);

      return resources;
    }

    void Material::fill_pso_desc(PipelineStateDesc& desc)
    {
      desc.primitive_topology = to_topology_type(m_primitive_topology);
      desc.vertex_shader = m_shader_pipeline.vs.get();
      desc.pixel_shader = m_shader_pipeline.ps.get();
      desc.root_signature = m_root_signature.get();
      desc.raster_state = m_raster_state;
      desc.blend_state = m_blend;
      desc.depth_stencil_state = m_depth_stencil;
    }

    void Material::validate_input_layout(InputLayout* inputLayout)
    {
      REX_UNUSED_PARAM(inputLayout);

      REX_ASSERT_X(inputLayout->validate_desc(m_input_layout_desc), "Input layout is not compatible with material");
    }


  }
}