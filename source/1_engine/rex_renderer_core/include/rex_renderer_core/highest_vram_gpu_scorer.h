#pragma once

#include "gpu_scorer.h"

namespace rex
{
  class HighestVramGpuScorer : public GpuScorer
  {
  public:
    const Gpu* highest_scoring_gpu(const rsl::vector<rsl::unique_ptr<Gpu>>& gpus) const override;
  };
} // namespace rex