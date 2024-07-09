#pragma once

#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
  namespace gfx
  {
    // Base class for a root signature, this will likely change in the future
    class RootSignature
    {
    public:
      virtual ~RootSignature() = default;

      virtual s32 param_idx_for_textures(ShaderType type) = 0;
      virtual s32 param_idx_for_samplers(ShaderType type) = 0;
    };
  }
}