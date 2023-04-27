#include "rex_renderer_core/highest_vram_gpu_scorer.h"

#include "rex_renderer_core/gpu.h"
#include "rex_std/algorithm.h"

namespace rex
{
  //-------------------------------------------------------------------------
  const Gpu* HighestVramGpuScorer::highest_scoring_gpu(const rsl::vector<rsl::unique_ptr<Gpu>>& gpus) const
  {
    auto it = std::max_element(gpus.cbegin(), gpus.cend(),
                               [](const rsl::unique_ptr<Gpu>& lhs, const rsl::unique_ptr<Gpu>& rhs)
                               {
                                 const size_t lhs_vram = lhs->description().dedicated_video_memory.size_in_bytes();
                                 const size_t rhs_vram = rhs->description().dedicated_video_memory.size_in_bytes();

                                 return rhs_vram > lhs_vram;
                               });

    return it != gpus.cend() ? it->get() : nullptr;
  }
} // namespace rex