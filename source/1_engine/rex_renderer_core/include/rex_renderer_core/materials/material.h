#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/shader_reflection/shader_signature.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"

#include "rex_renderer_core/materials/parameters/mat_sampler_parameter.h"
#include "rex_renderer_core/materials/parameters/mat_texture_parameter.h"

#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/gfx/render_context.h"

namespace rex
{
  namespace gfx
  {
    struct MaterialConstructSettings;

    // Holds the collection of all the resources for a single shader
    struct AllShaderResources
    {
      s32 textures_root_param_idx; // The param index in the root signature for the textures in this shader
      s32 samplers_root_param_idx; // The param index in the root signature for the samplers in this shader

      rsl::vector<TextureMaterialParameter*> textures;  // All the texture parameters for this shader
      rsl::vector<SamplerMaterialParameter*> samplers;  // All the sampler parameters for this shader
    };

    // Material construct settings are a wrapper around all the settings a material needs to store
    struct MaterialConstructSettings
    {
      RasterStateDesc raster_state;   // the rasterizer settings that'll be stored inside the material
      BlendDesc blend;								// the blend settings that'll be stored inside the material
      DepthStencilDesc depth_stencil;	// the depth stencil settings that'll be stored inside the material
    };

    // A material is an object combining all shaders and the parameters that can be set on them
    class Material
    {
    public:
      Material(ShaderPipeline&& shaderPipeline, const MaterialConstructSettings& matConstructSettings);

      // Set a texture parameter of the material
      void set_texture(rsl::string_view name, Texture2D* texture);
      // Set a sampler parameter of the material
      void set_sampler(rsl::string_view name, Sampler2D* sampler);
      // Set the blend factor of the material
      void set_blend_factor(const BlendFactor& blendFactor);

      // Return the primitive topology of this material
      PrimitiveTopology primitive_topology() const;
      // Return the root signature of this material
      RootSignature* root_signature();
      // Return the blend factor of this material
      BlendFactor blend_factor();

      // Return all the shader resources for a given shader type
      AllShaderResources resources_for_shader(ShaderType type);

      // Fill in the pso desc with data that's stored in the material
      void fill_pso_desc(PipelineStateDesc& desc);

      // Validate an input layout if it can be used with this material
      void validate_input_layout(InputLayout* inputLayout);

    private:
      // Initialize the parameters of the material based on the resources of a shader signature
      void init_parameters_from_shader_signature(ShaderType type, const ShaderSignature* signature);

    private:
      // All texture parameters of the material
      rsl::unordered_map<rsl::string, TextureMaterialParameter> m_textures;
      // All sampler parameters of the material
      rsl::unordered_map<rsl::string, SamplerMaterialParameter> m_samplers;

      // The primitive topology of the material
      PrimitiveTopology m_primitive_topology;
      // The root signature of the material
      rsl::unique_ptr<RootSignature> m_root_signature;

      // The rasterizer settings of the material
      RasterStateDesc m_raster_state;
      // The blend settings of the material
      BlendDesc m_blend;
      // The depth stencil settings of the material
      DepthStencilDesc m_depth_stencil;
      // The input layout desc of the material, used for validating other input layouts
      InputLayoutDesc m_input_layout_desc;
      // The blend factor of the material
      BlendFactor m_blend_factor;
      // The shader pipeline of the material
      ShaderPipeline m_shader_pipeline;
    };
  }
}