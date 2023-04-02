#pragma once

#include "rex_std/vector.h"
#include "rex_std/memory.h"

namespace pic
{
    class Gpu;

    using GpuVec = rsl::vector<rsl::unique_ptr<Gpu>>;

    class GpuManager
    {
    public:
        GpuManager() = default;
        virtual ~GpuManager() = default;

        virtual Gpu* selected() const = 0;
        virtual Gpu* first() const = 0;
        virtual const GpuVec& all() const = 0;
    };
}