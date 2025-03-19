#pragma once

#include "rex_std/string.h"

#include <d3d12.h>

namespace rex
{
  namespace gfx
  {
    // Return the shader model as a string
    // We cannot use enum reflection due to the arbitrary values assigned to the enum values
    rsl::string_view shader_model_name(D3D_SHADER_MODEL shaderModel);

    // Check if the given shader model is supported on the device
    bool check_for_shader_model_support(ID3D12Device* device, D3D_SHADER_MODEL version);

    // Query the max available shader model supported on the given device
    D3D_SHADER_MODEL query_shader_model_version(ID3D12Device* device);
  } // namespace gfx
} // namespace rex