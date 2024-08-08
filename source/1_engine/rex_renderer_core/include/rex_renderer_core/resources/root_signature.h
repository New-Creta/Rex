#pragma once

#include "rex_renderer_core/system/shader_type.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace gfx
  {
    class Shader;

    struct RootSignatureDesc2
    {
      Shader* vs;
      Shader* ps;
    };

    // A root signature holds all the parameters for every shader type bound to a pipeline.
    // It only specifies the type, not the actual data
    // for example:
    //
    // -- VERTEX SHADER --
    //   
    // cbuffer my_vertex_buffer : register(b0)
    // {
    //    .. some data here ..
    // };
    //
    // .. other vertex shader code ..
    //
    // 
    // 
    // -- PIXEL SHADER -- 
    //
    // SamplerState sampler0 : register(s0);
    // Texture2D texture0 : register(t0);
    //
    // .. other pixel shader code ..
    //
    //
    // The above would result in a root signature defining
    // a constant buffer, a sampler and a texture 2d as its parameters.
    class RootSignature
    {
    public:
      virtual ~RootSignature() = default;

      // Return the parameter index that's used for the textures for the shader of a given type
      virtual s32 param_idx_for_textures(ShaderType type) = 0;
      // Return the parameter index that's used for the samplers for the shader of a given type
      virtual s32 param_idx_for_samplers(ShaderType type) = 0;
    };
  }
}