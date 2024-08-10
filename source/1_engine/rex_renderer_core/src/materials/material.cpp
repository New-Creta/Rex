#include "rex_renderer_core/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/materials/material_system.h"

#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"
#include "rex_renderer_core/system/root_signature_cache.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogMaterial);

    Material::Material(const ShaderPipeline& shaderPipeline, const MaterialDesc& matDesc)
      : m_shader_pipeline(shaderPipeline)
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

      ShaderPipelineReflection& reflection = shader_reflection_cache::load(m_shader_pipeline);
      m_root_signature = root_signature_cache::load(m_shader_pipeline);
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

    void Material::set_blend_factor(const BlendFactor& blendFactor)
    {
      m_blend_factor = blendFactor;
    }

    void Material::bind_to(RenderContext* ctx)
    {
      ctx->set_primitive_topology(m_primitive_topology);
      ctx->set_root_signature(m_root_signature);
      ctx->set_blend_factor(m_blend_factor);

      const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = m_parameters_store->params();
      for (const auto& shader_resource : shader_params)
      {
        shader_resource->bind_to(ctx);
      }
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