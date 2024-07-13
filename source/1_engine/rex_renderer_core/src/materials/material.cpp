#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/rhi.h"

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
      m_input_layout = rhi::create_input_layout(shader_pipeline_reflection.vs->input_params());


      PipelineStateDesc pso_desc{};
      pso_desc.primitive_topology = to_topology_type(m_primitive_topology);
      pso_desc.vertex_shader = m_shader_pipeline.vs.get();
      pso_desc.pixel_shader = m_shader_pipeline.ps.get();
      pso_desc.root_signature = m_root_signature.get();
      pso_desc.input_layout = m_input_layout.get();

      // Raster state
      RasterStateDesc rasterizer_desc{};
      rasterizer_desc.fill_mode = FillMode::Solid;
      rasterizer_desc.cull_mode = CullMode::None;
      rasterizer_desc.front_ccw = false;
      rasterizer_desc.depth_bias = 0;
      rasterizer_desc.depth_bias_clamp = 0.0f;
      rasterizer_desc.sloped_scale_depth_bias = 0.0f;
      rasterizer_desc.depth_clip_enable = true;
      rasterizer_desc.multisample_enable = false;
      rasterizer_desc.aa_lines_enable = false;
      rasterizer_desc.forced_sample_count = 0;
      m_raster_state = rsl::make_unique<RasterState>(rasterizer_desc);;
      pso_desc.raster_state = *m_raster_state.get();

      // Blend State
      pso_desc.blend_state = BlendDesc();
      BlendDesc& blend_state = pso_desc.blend_state.value();
      blend_state.enable_alpha_to_coverage = false;
      blend_state.render_target[0].blend_enable = true;
      blend_state.render_target[0].src_blend = Blend::SrcAlpha;
      blend_state.render_target[0].dst_blend = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op = BlendOp::Add;
      blend_state.render_target[0].src_blend_alpha = Blend::One;
      blend_state.render_target[0].dst_blend_alpha = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op_alpha = BlendOp::Add;
      blend_state.render_target[0].render_target_write_mask = RenderTargetWriteMask::All;

      // depth stencil state
      pso_desc.depth_stencil_state = DepthStencilDesc();
      DepthStencilDesc& depth_stencil_desc = pso_desc.depth_stencil_state.value();
      depth_stencil_desc.depth_enable = false;
      depth_stencil_desc.depth_write_mask = DepthWriteMask::DepthWriteMaskAll;
      depth_stencil_desc.depth_func = ComparisonFunc::Always;
      depth_stencil_desc.stencil_enable = false;
      depth_stencil_desc.front_face.stencil_fail_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_depth_fail_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_pass_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_func = ComparisonFunc::Always;
      depth_stencil_desc.back_face = depth_stencil_desc.front_face;

      m_pso = rhi::create_pso(pso_desc);
    }

    void Material::init_parameters_from_shader_signature(ShaderType type, const ShaderSignature* signature)
    {
      for (const BoundResourceReflection& texture : signature->textures())
      {
        TextureMaterialParameter tex_param(texture.name, type, texture.shader_register);
        m_textures.emplace(texture.name, rsl::move(tex_param));
      }

      for (const BoundResourceReflection& sampler : signature->samplers())
      {
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

    ShaderResources Material::resources_for_shader(ShaderType type)
    {
      ShaderResources resources{};

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