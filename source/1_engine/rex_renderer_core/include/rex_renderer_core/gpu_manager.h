#pragma once

#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  class Gpu;

  using GpuVec = rsl::vector<rsl::unique_ptr<Gpu>>;

  class GpuManager
  {
  public:
    GpuManager()          = default;
    virtual ~GpuManager() = default;

    virtual const Gpu* selected() const = 0;
    virtual const Gpu* first() const    = 0;
    virtual const GpuVec& all() const   = 0;
  };
} // namespace rex