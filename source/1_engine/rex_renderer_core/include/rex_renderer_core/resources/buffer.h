#pragma once

#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    class Buffer
    {
    public:
      Buffer(rsl::memory_size size);
      virtual ~Buffer() = default;

    private:
      rsl::memory_size m_size;
    };
  }
}