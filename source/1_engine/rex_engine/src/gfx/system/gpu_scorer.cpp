#include "rex_engine/gfx/system/gpu_scorer.h"

namespace rex
{
  s64 calc_gpu_score(const GpuDescription& gpu)
  {
    return gpu.dedicated_video_memory.size_in_bytes();
  }
}