#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/shader_reflection/shader_reflection.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler.h"

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

    // A material is an object combining all shaders and the parameters that can be set on them
    // A material cannot be used directly, an instance needs to be created instead which can
    // set a shader's parameters at runtime
    class Material
    {
    public:
      Material(ShaderPipeline&& shaderPipeline, const ShaderPipelineReflection& shaderPipelineRefl);

      // Bind a constant buffer resource to a particular constant buffer parameter of the material
      void bind_constant_buffer(rsl::string_view name, ConstantBuffer* cb);
      // Bind a texture resource to a particular texture parameter of the material
      void bind_texture2d(rsl::string_view name, Texture2D* texture);
      // Bind a sampler resource to a particular sampler parameter of the material
      void bind_sampler(rsl::string_view name, Sampler* sampler);

      // Update a bound constant buffer's data
      void set_constant_buffer_data(rsl::string_view name, const void* data, s32 size);

    private:
      void store_shader_params(const ShaderSignature* signature);

    private:
      rsl::unordered_map<rsl::string_view, ConstantBufferReflection> m_constant_buffers;
      rsl::unordered_map<rsl::string_view, ShaderParamReflection>   m_input_params;
      rsl::unordered_map<rsl::string_view, ShaderParamReflection>   m_output_params;
      rsl::unordered_map<rsl::string_view, BoundResourceReflection> m_samplers;
      rsl::unordered_map<rsl::string_view, BoundResourceReflection> m_textures;


      ShaderPipeline m_shader_pipeline;
    };

    class MaterialInstance
    {

    };
  }
}