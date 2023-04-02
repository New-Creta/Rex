#include "rex_renderer_core/highest_vram_gpu_scorer.h"
#include "rex_renderer_core/gpu.h"

namespace rex
{
    //-------------------------------------------------------------------------
    Gpu* HighestVramGpuScorer::get_highest_scoring_gpu(const rsl::vector<rsl::unique_ptr<Gpu>>& gpus) const
    {
        auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
            [](const auto& lhs, const auto& rhs)
            {
                size_t lhs_vram = lhs->get_description().dedicated_video_memory.bytes();
                size_t rhs_vram = rhs->get_description().dedicated_video_memory.bytes();

                return rhs_vram > lhs_vram;
            });

        return it != gpus.cend()
            ? it->get()
            : nullptr;
    }
}