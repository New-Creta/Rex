#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/shaders/shader_type.h"
#include "rex_std/bonus/string.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace rhi
  {
    struct CompileShaderDesc
    {
      CompileShaderDesc() = default;
      CompileShaderDesc(rsl::string_view shaderName, rex::rhi::ShaderType shaderType, rsl::string_view filePath);

      ShaderType shader_type = ShaderType::None;
      rsl::string_view shader_entry_point;
      rsl::small_stack_string shader_name;
      rsl::tiny_stack_string shader_feature_target;
      memory::Blob shader_code;
    };
  } // namespace rhi
} // namespace rex