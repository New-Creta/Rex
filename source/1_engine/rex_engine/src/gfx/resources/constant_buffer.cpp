#include "rex_engine/gfx/resources/constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    ConstantBuffer::ConstantBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size size)
      : Buffer(size)
      , m_gpu_address(gpuAddress)
      , m_view(view)
    {}
    ResourceView* ConstantBuffer::resource_view()
    {
      return m_view;
    }
    u64 ConstantBuffer::gpu_address() const
    {
      return m_gpu_address;
    }
  }
}