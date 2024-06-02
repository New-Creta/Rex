#include "rex_renderer_core/system/gpu_scorer.h"

namespace rex
{
  s32 calc_gpu_score(const GpuDescription& gpu)
  {
    return gpu.dedicated_video_memory.size_in_bytes();
  }
}