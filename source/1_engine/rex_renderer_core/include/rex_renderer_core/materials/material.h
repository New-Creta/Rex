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
#include "rex_renderer_core/system/output_merger.h"

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
      ShaderPipeline shader_pipeline; // The shader pipeline to be used by the material
      OutputMergerDesc output_merger; // The configuration of the output merger of a material
    };

    // A material is an object combining all shaders and the parameters that can be set on them
    class Material
    {
    public:
      Material(const MaterialDesc& matDesc);

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

    private:
      // Initialize the material based on the desc
      void init(const MaterialDesc& matDesc);

    private:
      // The backend storage for all material parameters
      rsl::unique_ptr<ShaderParametersStore> m_parameters_store;

      // The primitive topology of the material
      PrimitiveTopology m_primitive_topology;
      // The root signature of the material
      RootSignature* m_root_signature;

      // The output merger settings of the material
      OutputMergerDesc m_output_merger;
      // The blend factor of the material, belongs to the output merger
      BlendFactor m_blend_factor;
      // The shader pipeline of the material
      ShaderPipeline m_shader_pipeline;
    };
  }
}