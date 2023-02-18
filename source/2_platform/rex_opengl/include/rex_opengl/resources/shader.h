#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/attributes.h"

namespace rex
{
  REX_NO_DISCARD u32 create_vertex_shaders(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength = nullptr);
  REX_NO_DISCARD u32 create_fragment_shaders(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength = nullptr);

  void destroy_vertex_shader(u32 shader);
  void destroy_fragment_shader(u32 shader);
} // namespace rex