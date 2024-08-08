#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/materials/material_system.h"

#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"
#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogMaterial);

    Material::Material(ShaderPipeline&& shaderPipeline, const MaterialDesc& matDesc)
      : m_shader_pipeline(rsl::move(shaderPipeline))
      , m_primitive_topology(PrimitiveTopology::TriangleList)
      , m_root_signature()
      , m_blend_factor()
    {
      init(matDesc);
    }

    void Material::init(const MaterialDesc& matDesc)
    {
      m_raster_state = matDesc.raster_state;
      m_blend = matDesc.blend;
      m_depth_stencil = matDesc.depth_stencil;

      ShaderPipelineReflection2& reflection = shader_reflection_cache::load(m_shader_pipeline);
      m_root_signature = rhi::create_root_signature(reflection.parameters);
      m_parameters_store = rsl::make_unique<ShaderParametersStore>(reflection.material_param_store_desc);
    }

    void Material::set(rsl::string_view name, ConstantBuffer* constantBuffer)
    {
      m_parameters_store->set(name, constantBuffer);
    }
    void Material::set(rsl::string_view name, Texture2D* texture)
    {
      m_parameters_store->set(name, texture);
    }
    void Material::set(rsl::string_view name, Sampler2D* sampler)
    {
      m_parameters_store->set(name, sampler);
    }

    rsl::vector<ShaderResource> Material::shader_resources()
    {
      return m_parameters_store->params();
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

    void Material::fill_pso_desc(PipelineStateDesc& desc)
    {
      desc.primitive_topology = to_topology_type(m_primitive_topology);
      desc.shader_pipeline = m_shader_pipeline;
      desc.raster_state = m_raster_state;
      desc.blend_state = m_blend;
      desc.depth_stencil_state = m_depth_stencil;
    }
  }
}