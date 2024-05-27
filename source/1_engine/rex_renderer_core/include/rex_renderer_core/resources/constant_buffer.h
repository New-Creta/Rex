#pragma once

#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace rhi
  {
    class ConstantBuffer : public Buffer
    {
    public:
      ConstantBuffer(rsl::memory_size size);

    private:

    };
  } // namespace rhi
} // namespace rex