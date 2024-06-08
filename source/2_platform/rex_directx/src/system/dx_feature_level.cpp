#include "rex_directx/system/dx_feature_level.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_std/array.h"

#include <Windows.h>

namespace rex
{
  //-------------------------------------------------------------------------
  rsl::string_view feature_level_name(D3D_FEATURE_LEVEL level)
  {
    switch(level)
    {
      case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0: return rsl::string_view("D3D_FEATURE_LEVEL_12_0");
      case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1: return rsl::string_view("D3D_FEATURE_LEVEL_12_1");
      case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE: return rsl::string_view("D3D_FEATURE_LEVEL_1_0_CORE");
      default: return rsl::string_view("Unknown feature level");
    }
  }
} // namespace rex