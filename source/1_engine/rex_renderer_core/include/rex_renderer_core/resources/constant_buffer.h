#pragma once

#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace gfx
  {
    // A constant buffer is a buffer holding data that's consistent for a draw call of an object
    // It often has various data to use in different shaders (eg. mwp matrix)
    class ConstantBuffer : public Buffer
    {
    public:
      ConstantBuffer(rsl::memory_size size);

    private:

    };
  } // namespace gfx
} // namespace rex