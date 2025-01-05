#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_engine/gfx/system/shader_type.h"
#include "rex_engine/gfx/resources/shader.h"
#include "rex_engine/gfx/shader_reflection/shader_signature.h"

#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"

#include "rex_engine/gfx/core/primitive_topology.h"
#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/root_signature.h"
#include "rex_engine/gfx/resources/blend_state.h"
#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/system/output_merger.h"

#include "rex_engine/gfx/system/shader_parameters_store.h"

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
    };

    // structure holding meta data about render pass overwriting
    // Materials are allowed to differ in shader pipeline from their render passes
    // But these do need to be tracked by the render pass themselves
    struct MaterialPsoOverwrite
    {
      bool is_overwritten;
      ShaderPipeline shader_pipeline;
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

      // Check if the material has the specified shader pipeline
      bool uses_shader_pipeline(const ShaderPipeline& pipeline) const;

      // Create the PSO which would overwrite the one of the render pass
      // Often this doesn't do anything but if a material gets created
      // with a different set of shaders than the render pass its used in
      // this function will create a cusom pso for the material
      MaterialPsoOverwrite load_pso_overwrite(const PipelineStateDesc& psoDesc);

    private:
      // The backend storage for all material parameters
      rsl::unique_ptr<ShaderParametersStore> m_parameters_store;

      // The pso of the material
      // It's possible this is null, it only gets created
      // if the shaders for the material differ than the ones of the renderpass
      // in which the material is used
      rsl::unique_ptr<PipelineState> m_pso;

      // The shader pipeline of the material
      ShaderPipeline m_shader_pipeline;
    };
  }
}