#pragma once

#include "rex_renderer_core/system/gpu_description.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  using GpuScorerFn = rsl::function<s32(const GpuDescription&)>;
  s32 calc_gpu_score(const GpuDescription& gpu)
  {
    return gpu.dedicated_video_memory.size_in_bytes();
  }
} // namespace rex