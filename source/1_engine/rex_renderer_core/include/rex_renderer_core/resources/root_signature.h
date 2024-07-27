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

    // Base class for a root signature, this will likely change in the future
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