#pragma once

#include "rex_std/vector.h"
#include "rex_std/memory.h"

namespace rex
{
	class Gpu;

	class GpuScorer
	{
	public:
		virtual ~GpuScorer() = default;

		virtual const Gpu* highest_scoring_gpu(const rsl::vector<rsl::unique_ptr<Gpu>>& gpus) const = 0;
	};
}