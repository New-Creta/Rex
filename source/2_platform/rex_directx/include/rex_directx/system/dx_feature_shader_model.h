#pragma once

#include "rex_std/string.h"

#include <d3d12.h>

namespace rex
{
  namespace rhi
  {
    rsl::string_view shader_model_name(D3D_SHADER_MODEL shaderModel);

    bool check_for_shader_model_support(ID3D12Device* device, D3D_SHADER_MODEL version);

    D3D_SHADER_MODEL query_shader_model_version(ID3D12Device* device);
  }
} // namespace rex