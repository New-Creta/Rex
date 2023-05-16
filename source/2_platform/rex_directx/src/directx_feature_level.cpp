#include "rex_directx/directx_feature_level.h"

#include "rex_engine/diagnostics/assert.h" // for REX_ASSERT
#include "rex_std/array.h"                 // for const_random_access_iterator

namespace rex
{
  const rsl::array g_expected_feature_levels = {D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1};

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

  //-------------------------------------------------------------------------
  bool is_correct_feature_level(D3D_FEATURE_LEVEL level)
  {
    return rsl::cend(g_expected_feature_levels) != rsl::find(rsl::cbegin(g_expected_feature_levels), rsl::cend(g_expected_feature_levels), level);
  }

  //-------------------------------------------------------------------------
  D3D_FEATURE_LEVEL query_feature_level(IDXGIAdapter* adapter)
  {
    // backwards looping as it's checking for a minimum feature level
    for(auto it = g_expected_feature_levels.crbegin(); it != g_expected_feature_levels.crend(); ++it)
    {
      const D3D_FEATURE_LEVEL feature_level = *it;
      if(SUCCEEDED(D3D12CreateDevice(adapter, feature_level, __uuidof(ID3D12Device), nullptr)))
      {
        return feature_level;
      }
    }

    REX_ASSERT("At least D3D_FEATURE_LEVEL_12_0 has to be supported for DirectX 12!");

    // If the compiler doesn't recognise D3D_FEATURE_LEVEL_1_0_CORE
    // Make sure you're using windows SDK 10.0.18362.0 or later
    return D3D_FEATURE_LEVEL_1_0_CORE;
  }
} // namespace rex