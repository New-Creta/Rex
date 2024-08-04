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
#include "rex_renderer_core/system/param_binding_indices.h"
#include "rex_renderer_core/system/view_param.h"

namespace rex
{
  namespace gfx
  {
    struct MaterialConstructSettings;
    struct ViewParam;
    struct ParamBindingIndices;

    // Holds the collection of all the resources for a single shader
    struct ShaderResourceView
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

		enum class ShaderParameterType2
		{
			ConstantBuffer,
			Texture2D,
			Sampler2D
		};

    struct ShaderResources
    {
      rsl::vector<ConstantBuffer*> constant_buffers;
      rsl::vector<Texture2D*> textures;
      rsl::vector<Sampler2D*> samplers;

      s32 cb_slot;
      s32 srv_slot;
      s32 sampler_slot;
    };

    class MaterialParameter2
    {
    public:
      MaterialParameter2(ShaderType type)
        : m_type(type)
      {}

      void set_resource(Resource* resource)
      {
        m_resource = resource;
      }

      Resource* resource()
      {
        return m_resource;
      }

      ShaderType type() const
      {
        return m_type;
      }

    private:
      Resource* m_resource;
      ShaderType m_type;
    };

    // A material is an object combining all shaders and the parameters that can be set on them
    class Material
    {
    public:
      Material(ShaderPipeline&& shaderPipeline, const MaterialConstructSettings& matConstructSettings);

      void set(rsl::string_view name, ConstantBuffer* constantBuffer);
      void set(rsl::string_view name, Texture2D* texture);
      void set(rsl::string_view name, Sampler2D* sampler);

      rsl::unordered_map<ShaderType, ShaderResources> shader_resources();

      // Set the blend factor of the material
      void set_blend_factor(const BlendFactor& blendFactor);

      // Return the primitive topology of this material
      PrimitiveTopology primitive_topology() const;
      // Return the root signature of this material
      RootSignature* root_signature();
      // Return the blend factor of this material
      BlendFactor blend_factor();

      // Return all the shader resources for a given shader type
      //ShaderResourceView resources_for_shader(ShaderType type);

      // Fill in the pso desc with data that's stored in the material
      void fill_pso_desc(PipelineStateDesc& desc);

      // Validate an input layout if it can be used with this material
      void validate_input_layout(InputLayout* inputLayout);

      void bind_resources(RenderContext* ctx);

    protected:
      s32 index_of_param(rsl::string_view name);

    private:
      void init();

      // Initialize the parameters of the material based on the resources of a shader signature
      void init_parameters_from_shader_signature(ShaderType type, const ShaderSignature& signature);

    private:
      // The parameters that will be tied directly to the shader
      // This holds a list of views or view tables
      //rsl::vector<ShaderParameter*> m_shader_parameters;

      // The material parameters that are exposed to the user
      // They're an indirection from the shader parameters
      //rsl::unordered_map<rsl::string, rsl::unique_ptr<MaterialParameter>> m_material_parameters;


      rsl::unordered_map<rsl::string, ViewParam> m_param_name_to_view_idx;
      rsl::unordered_map<ShaderType, ParamBindingIndices> m_param_binding_indices;
      rsl::vector<MaterialParameter2> m_constant_buffers;
      rsl::vector<MaterialParameter2> m_textures;
      rsl::vector<MaterialParameter2> m_samplers;




      // All texture parameters of the material
      //rsl::unordered_map<rsl::string, TextureMaterialParameter> m_textures;
      // All sampler parameters of the material
      //rsl::unordered_map<rsl::string, SamplerMaterialParameter> m_samplers;


      //rsl::unordered_map<rsl::string, rsl::unique_ptr<ShaderParameter>> m_shader_parameters;






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