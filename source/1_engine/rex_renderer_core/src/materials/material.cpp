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

    Material::Material(const MaterialDesc& matDesc)
      : m_shader_pipeline(matDesc.shader_pipeline)
      , m_primitive_topology(PrimitiveTopology::TriangleList)
      , m_root_signature()
      , m_blend_factor()
    {
      m_output_merger = matDesc.output_merger;

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
      if (m_pso.get())
      {
        ctx->set_pipeline_state(m_pso.get());
      }
      ctx->set_primitive_topology(m_primitive_topology);
      ctx->set_root_signature(m_root_signature);
      ctx->set_blend_factor(m_blend_factor);

      const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = m_parameters_store->params();
      for (const auto& shader_resource : shader_params)
      {
        shader_resource->bind_to(ctx);
      }
    }

    MaterialPsoOverwrite Material::load_pso_overwrite(const PipelineStateDesc& psoDesc)
    {
      MaterialPsoOverwrite result{};
      if (m_shader_pipeline != psoDesc.shader_pipeline)
      {
        result.is_overwritten = true;
        result.pso_desc = psoDesc;
        result.pso_desc.shader_pipeline = m_shader_pipeline;
      }

      return result;
    }

  }
}