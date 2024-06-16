#include "rex_renderer_core/system/fence.h"

namespace rex
{
  namespace gfx
  {
    Fence::Fence()
      : m_cpu_value(0)
    {}

    u64 Fence::cpu_value() const
    {
      return m_cpu_value;
    }

    void Fence::inc_cpu_val()
    {
      m_cpu_value++;
    }
  }
}