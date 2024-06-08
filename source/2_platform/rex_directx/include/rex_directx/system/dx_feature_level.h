#pragma once

#include "rex_directx/dxgi/includes.h"
#include "rex_std/string.h"

#include <d3d12.h>

namespace rex
{
  rsl::string_view feature_level_name(D3D_FEATURE_LEVEL level);
} // namespace rex