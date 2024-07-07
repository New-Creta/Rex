#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogMaterial);

    //
    // Shaders take multiple levels of data
    // 1. Per View Data
    // This data is shared by all materials used in a view. Should contain all information like camera matrices, light directions, time, ..
    // 
    // 2. Per Pass Data
    // This data is shared by all materials within the same pass. This is only relevant once we have multiple render passes
    // 
    // 3. Per material data
    // This data contains information per material like textures
    // 
    // 4. Per instance data
    // Contains data that's separate per instance. For example a world view projection matrix
    //





    Material::Material(ShaderPipeline&& shaderPipeline)
      : m_shader_pipeline(rsl::move(shaderPipeline))
      , m_primitive_topology(PrimitiveTopology::TriangleList)
      , m_pso()
      , m_root_signature()
      , m_blend_factor()
    {
      // We calculate the shader reflection so we know which parameters this material needs
      ShaderPipelineReflection shader_pipeline_reflection = reflect_shader_pipeline(m_shader_pipeline);

      init_parameters_from_shader_signature(ShaderType::Vertex, shader_pipeline_reflection.vs.get());
      init_parameters_from_shader_signature(ShaderType::Pixel, shader_pipeline_reflection.ps.get());

      m_root_signature = rhi::create_root_signature(shader_pipeline_reflection);
    }

    void Material::init_parameters_from_shader_signature(ShaderType type, const ShaderSignature* signature)
    {
      for (const BoundResourceReflection& texture : signature->textures())
      {
        TextureMaterialParameter tex_param(texture.name(), texture.shader_register());
        m_textures.emplace(texture.name(), rsl::move(tex_param));
      }

      for (const BoundResourceReflection& sampler : signature->samplers())
      {
        SamplerMaterialParameter tex_param(sampler.name(), sampler.shader_register());
        m_samplers.emplace(sampler.name(), rsl::move(tex_param));
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

    PrimitiveTopology Material::primitive_topology() const
    {
      return m_primitive_topology;
    }
    PipelineState* Material::pso()
    {
      return m_pso.get();
    }
    RootSignature* Material::root_signature()
    {
      return m_root_signature.get();
    }
    BlendFactor Material::blend_factor()
    {
      return m_blend_factor;
    }




    //void Material::bind_constant_buffer(rsl::string_view name, ConstantBuffer* cb)
    //{
    //  REX_ASSERT_X(m_constant_buffers.contains(name), "Trying to bind a constant buffer to a name that doesn't exist: {}", name);
    //}
    //void Material::bind_texture2d(rsl::string_view name, Texture2D* texture)
    //{

    //}
    ////void Material::bind_sampler(rsl::string_view name, Sampler* sampler)
    ////{

    ////}

    //void Material::set_constant_buffer_data(rsl::string_view name, const void* data, s32 size)
    //{

    //}

    //void Material::store_shader_params(const ShaderSignature* signature)
    //{
    //  if (!signature)
    //  {
    //    return;
    //  }

    //  // Constant buffers
    //  //const auto& new_constant_buffers = signature->constant_buffers();
    //  //m_constant_buffers.reserve(m_constant_buffers.capacity() + new_constant_buffers.size());
    //  //rsl::copy(new_constant_buffers.cbegin(), new_constant_buffers.cend(), m_constant_buffers.end());

    //  // Textures
    //  //const auto& new_textures = signature->textures();
    //  //m_textures.reserve(m_textures.capacity() + new_textures.size());
    //  //rsl::copy(new_textures.cbegin(), new_textures.cend(), m_textures.end());

    //  // Samplers
    //  //const auto& new_samplers = signature->samplers();
    //  //m_samplers.reserve(m_samplers.capacity() + new_samplers.size());
    //  //rsl::copy(new_samplers.cbegin(), new_samplers.cend(), m_samplers.end());
    //}
  }
}