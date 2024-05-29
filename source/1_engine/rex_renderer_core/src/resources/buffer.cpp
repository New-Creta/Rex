#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace rhi
  {
    Buffer::Buffer(rsl::memory_size size)
      : m_size(size)
    {}

    rsl::memory_size Buffer::size() const
    {
      return m_size;
    }
  }
}