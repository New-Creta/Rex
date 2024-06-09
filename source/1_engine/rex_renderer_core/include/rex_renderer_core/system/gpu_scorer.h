#pragma once

#include "rex_renderer_core/system/gpu_description.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  using GpuScorerFn = rsl::function<s64(const GpuDescription&)>;
  s64 calc_gpu_score(const GpuDescription& gpu);
} // namespace rex