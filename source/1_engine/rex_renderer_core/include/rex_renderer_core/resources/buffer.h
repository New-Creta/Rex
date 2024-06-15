#pragma once

#include "rex_std/bonus/memory.h"

#include "rex_renderer_core/resources/resource.h"

namespace rex
{
  namespace rhi
  {
    // A 1D array on the gpu
    class Buffer : public Resource
    {
    public:
      Buffer(rsl::memory_size size);
      virtual ~Buffer() = default;

      // The total size of the buffer
      rsl::memory_size size() const;

    private:
      rsl::memory_size m_size;
    };
  }
}