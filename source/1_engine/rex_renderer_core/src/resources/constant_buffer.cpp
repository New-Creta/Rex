#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    ConstantBuffer::ConstantBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size size)
      : Buffer(size)
      , m_gpu_address(gpuAddress)
      , m_view(view)
    {}
  }
}