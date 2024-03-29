#pragma once

#include "rex_opengl/resources/buffer.h"

namespace rex
{
  class ElementBuffer : public Buffer
  {
  public:
    ElementBuffer();

    void set_data(u64 size, const void* data, BufferUsage usage = BufferUsage::StaticDraw);
  };
} // namespace rex