#pragma once

#include "rex_engine/types.h"
#include "rex_std/string.h"

struct ID3D12Device;

namespace rex
{
  struct ShaderModelInfo
  {
    s32 version;
    rsl::string name;
  };

  rsl::string shader_model_name(s32 shaderModel);

  bool check_for_shader_model_support(ID3D12Device* device, s32 version);

  ShaderModelInfo query_shader_model_version(ID3D12Device* device);
} // namespace rex