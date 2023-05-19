#pragma once

#include "rex_std/string.h"

#include <d3d12.h>
#include <dxgi.h>

namespace rex
{
  rsl::string_view feature_level_name(D3D_FEATURE_LEVEL level);

  bool is_correct_feature_level(D3D_FEATURE_LEVEL level);

  D3D_FEATURE_LEVEL query_feature_level(IDXGIAdapter* adapter);
} // namespace rex