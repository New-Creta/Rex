#pragma once

#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/shader_reflection/shader_reflection.h"

namespace rex
{
  namespace gfx
  {
    // NOTE: A parameter with the same name and size between shaders
    // is treated as a parameters that's shared between these shaders

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
      Material(const MaterialSignature& rootSignate);

      void set_constant_buffer(rsl::string_view name, const void* data, s32 size);
      void set_texture(rsl::string_view name, DescriptorHandle view);
      void set_sampler(rsl::string_view name, DescriptorHandle view);

    private:
      void store_shader_params(ShaderSignature* signature)
      {
        
      }

    private:
      rsl::vector<ConstantBufferReflection> m_constant_buffers;
      rsl::vector<ShaderParamReflection>   m_input_params;
      rsl::vector<ShaderParamReflection>   m_output_params;
      rsl::vector<BoundResourceReflection> m_samplers;
      rsl::vector<BoundResourceReflection> m_textures;


      rsl::unordered_map<rsl::string_view, 
    };

    class MaterialInstance
    {

    };
  }
}