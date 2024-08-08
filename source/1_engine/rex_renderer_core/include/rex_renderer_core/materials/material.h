#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/shader_reflection/shader_signature.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/gfx/render_context.h"
#include "rex_renderer_core/system/view_param.h"

#include "rex_renderer_core/system/shader_parameters_store.h"

namespace rex
{
  namespace gfx
  {
    struct MaterialDesc;
    struct ViewParam;
    struct ParamBindingSlots;
    class ShaderParameter;

    // Material construct settings are a wrapper around all the settings a material needs to store
    struct MaterialDesc
    {
      RasterStateDesc raster_state;   // the rasterizer settings that'll be stored inside the material
      BlendDesc blend;								// the blend settings that'll be stored inside the material
      DepthStencilDesc depth_stencil;	// the depth stencil settings that'll be stored inside the material
    };

    // A material is an object combining all shaders and the parameters that can be set on them
    class Material
    {
    public:
      Material(const ShaderPipeline& shaderPipeline, const MaterialDesc& matDesc);

      // Set a material parameter to a new resource
      void set(rsl::string_view name, ConstantBuffer* constantBuffer);
      void set(rsl::string_view name, Texture2D* texture);
      void set(rsl::string_view name, Sampler2D* sampler);

      // Return all shader parameters of a material
      const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params() const;

      // Set the blend factor of the material
      void set_blend_factor(const BlendFactor& blendFactor);

      // Bind a material to a render ctx
      void bind_to(RenderContext* ctx);

      // Fill in the pso desc with data that's stored in the material
      void fill_pso_desc(PipelineStateDesc& desc);

    private:
      // Initialize the material based on the desc
      void init(const MaterialDesc& matDesc);

    private:
      // The backend storage for all material parameters
      rsl::unique_ptr<ShaderParametersStore> m_parameters_store;

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
      // The blend factor of the material
      BlendFactor m_blend_factor;
      // The shader pipeline of the material
      ShaderPipeline m_shader_pipeline;
    };
  }
}