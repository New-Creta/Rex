#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/shaders/shader_type.h"

namespace rex
{
  namespace rhi
  {
    struct LoadShaderDesc
    {
      LoadShaderDesc()
        : shader_type(ShaderType::None)
        , shader_byte_code()
        , constant_buffer_count(0)
      {
      }

      ShaderType shader_type;
      memory::Blob shader_byte_code;
      s32 constant_buffer_count;
    };
  }
}
