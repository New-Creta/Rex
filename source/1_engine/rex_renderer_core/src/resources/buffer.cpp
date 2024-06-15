#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace gfx
  {
    Buffer::Buffer(rsl::memory_size size)
      : m_size(size)
    {}

    // The total size of the buffer
    rsl::memory_size Buffer::size() const
    {
      return m_size;
    }
  }
}