#pragma once

#include "rex_engine/gfx/resources/buffer.h"
#include "rex_engine/engine/types.h"

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
      ConstantBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size size);

      // Return the resource view of a constant buffer
      ResourceView* resource_view();
      // Return the gpu address of a constant buffer
      u64 gpu_address() const;

    private:
      ResourceView* m_view;
      u64 m_gpu_address;
    };
  } // namespace gfx
} // namespace rex