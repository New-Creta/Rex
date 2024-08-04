#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    ConstantBuffer::ConstantBuffer(ResourceView* view, rsl::memory_size size)
      : Buffer(size)
      , m_view(view)
    {}
  }
}