#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace rhi
  {
    ConstantBuffer::ConstantBuffer(rsl::memory_size size)
      : Buffer(size)
    {}
  }
}