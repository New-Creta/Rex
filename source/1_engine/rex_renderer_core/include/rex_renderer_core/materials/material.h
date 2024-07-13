#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/shader_reflection/shader_reflection.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"

#include "rex_renderer_core/materials/material_parameter.h"

#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/gfx/render_context.h"

namespace rex
{
  namespace gfx
  {
    // NOTE: A parameter with the same name and size between shaders
    // is treated as a parameters that's shared between these shaders
    // In DirectX a param can be visible in 1 shader or in all of them, you can't mask combine

    // A signature describes all the resources that need to be bound for a shader
    struct MaterialSignature
    {
      rsl::unique_ptr<ShaderSignature> vs;
      rsl::unique_ptr<ShaderSignature> ps;
    };

    struct MaterialDesc
    {
      Shader* vs;
      Shader* ps;
    };

    struct ShaderResources
    {
      s32 textures_root_param_idx;
      s32 samplers_root_param_idx;

      rsl::vector<TextureMaterialParameter*> textures;
      rsl::vector<SamplerMaterialParameter*> samplers;
    };

    // A material is an object combining all shaders and the parameters that can be set on them
    // A material cannot be used directly, an instance needs to be created instead which can
    // set a shader's parameters at runtime
    class Material
    {
    public:
      Material(ShaderPipeline&& shaderPipeline);

      void set_texture(rsl::string_view name, Texture2D* texture);
      void set_sampler(rsl::string_view name, Sampler2D* sampler);

      PrimitiveTopology primitive_topology() const;
      PipelineState* pso();
      RootSignature* root_signature();
      BlendFactor blend_factor();

      ShaderResources resources_for_shader(ShaderType type);

    private:
      void init_parameters_from_shader_signature(ShaderType type, const ShaderSignature* signature);

    private:
      rsl::unordered_map<rsl::string, TextureMaterialParameter> m_textures;
      rsl::unordered_map<rsl::string, SamplerMaterialParameter> m_samplers;

      PrimitiveTopology m_primitive_topology;
      rsl::unique_ptr<PipelineState> m_pso;
      rsl::unique_ptr<RootSignature> m_root_signature;
      rsl::unique_ptr<InputLayout> m_input_layout;
      rsl::unique_ptr<RasterState> m_raster_state; // The render state used by imgui

      BlendFactor m_blend_factor;

      ShaderPipeline m_shader_pipeline;
    };

    class MaterialInstance
    {

    };
  }
}