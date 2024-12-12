#include "rex_engine/gfx/materials/material.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/materials/material_system.h"

#include "rex_engine/gfx/shader_reflection/shader_reflection_cache.h"
#include "rex_engine/gfx/system/root_signature_cache.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogMaterial);

    Material::Material(const MaterialDesc& matDesc)
      : m_shader_pipeline(matDesc.shader_pipeline)
    {
      const ShaderPipelineReflection& reflection = shader_reflection_cache::load(m_shader_pipeline);
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

    void Material::bind_to(RenderContext* ctx)
    {
      if (m_pso.get())
      {
        ctx->set_pipeline_state(m_pso.get());
				ctx->set_root_signature(m_pso->root_signature());
      }

      const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = m_parameters_store->params();
      for (const auto& shader_resource : shader_params)
      {
        shader_resource->bind_to(ctx);
      }
    }

    bool Material::uses_shader_pipeline(const ShaderPipeline& pipeline) const
    {
      return m_shader_pipeline == pipeline;
    }

    MaterialPsoOverwrite Material::load_pso_overwrite(const PipelineStateDesc& psoDesc)
    {
      MaterialPsoOverwrite result{};
      if (m_shader_pipeline != psoDesc.shader_pipeline)
      {
        if (m_pso == nullptr)
        {
          PipelineStateDesc pso_desc = psoDesc;
          pso_desc.shader_pipeline = m_shader_pipeline;
          m_pso = gal()->create_pso(pso_desc);
        }

        result.is_overwritten = true;
        result.shader_pipeline = m_shader_pipeline;
      }

      return result;
    }

  }
}