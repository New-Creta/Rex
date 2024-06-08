#pragma once

#include "rex_std/vector.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  // Removes an element from src at given and index and adds it to the end of dst
  template <typename T>
  T& transfer_object(s32 idx, rsl::vector<T>& src, rsl::vector<T>& dst)
  {
    auto ctx = rsl::move(src[idx]);
    auto it = src.begin() + idx;
    src.erase(it);
    dst.emplace_back(rsl::move(ctx));
    return dst.back();
  }
}