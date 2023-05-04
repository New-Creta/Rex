#pragma once

#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  class Gpu;

  using GpuScorerFn = rsl::function<const Gpu*(const rsl::vector<rsl::unique_ptr<Gpu>>&)>;
} // namespace rex