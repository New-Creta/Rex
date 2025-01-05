#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_engine/gfx/system/shader_type.h"
#include "rex_std/bonus/string.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace gfx
  {
    struct CompileShaderDesc
    {
      ShaderType shader_type = ShaderType::None;
      rsl::string_view shader_entry_point;
      rsl::small_stack_string shader_name;
      rsl::tiny_stack_string shader_feature_target;
      rsl::string_view shader_source_code;
    };
  } // namespace gfx
} // namespace rex