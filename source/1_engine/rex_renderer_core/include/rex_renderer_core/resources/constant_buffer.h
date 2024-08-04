#pragma once

#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace gfx
  {
    class ResourceView;

    // A constant buffer is a buffer holding data that's consistent for a draw call of an object
    // It often has various data to use in different shaders (eg. mwp matrix)
    class ConstantBuffer : public Buffer
    {
    public:
      ConstantBuffer(ResourceView* view, rsl::memory_size size);

      ResourceView* resource_view()
      {
        return m_view;
      }

    private:
      ResourceView* m_view;
    };
  } // namespace gfx
} // namespace rex