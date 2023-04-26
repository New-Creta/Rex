#pragma once

#include "rex_engine/types.h"
#include "rex_std/string.h"

struct IDXGIAdapter;

namespace rex
{
  struct FeatureLevelInfo
  {
    s32 level;
    rsl::string name;
  };

  bool is_correct_feature_level(s32 level);

  FeatureLevelInfo query_feature_level(IDXGIAdapter* adapter);
} // namespace rex